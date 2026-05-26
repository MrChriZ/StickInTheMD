#include "stickinthemd/pdf_exporter.hpp"

#include <cstdlib>
#include <sstream>

namespace stickinthemd {

namespace {

bool command_exists(const std::string &command) {
#if defined(_WIN32)
  const std::string check = "where " + command + " >nul 2>&1";
#else
  const std::string check = "command -v " + command + " >/dev/null 2>&1";
#endif
  return std::system(check.c_str()) == 0;
}

std::string shell_quote(const std::filesystem::path &path) {
  const auto native = path.string();
#if defined(_WIN32)
  return "\"" + native + "\"";
#else
  std::string quoted = "'";
  for (char c : native) {
    if (c == '\'') {
      quoted += "'\\''";
    } else {
      quoted += c;
    }
  }
  quoted += "'";
  return quoted;
#endif
}

PdfExportResult run_command(const std::string &cmd, const std::string &method) {
  PdfExportResult result;
  result.method = method;
  if (std::system(cmd.c_str()) != 0) {
    result.error = "External PDF tool failed.";
    return result;
  }
  result.ok = true;
  return result;
}

} // namespace

PdfExportResult
PdfExporter::export_html_file(const std::filesystem::path &html_path,
                              const std::filesystem::path &pdf_path) const {
  PdfExportResult result;
  if (!std::filesystem::exists(html_path)) {
    result.error = "HTML source file is missing.";
    return result;
  }

  const auto html_q = shell_quote(html_path);
  const auto pdf_q = shell_quote(pdf_path);

#if defined(_WIN32)
  const char *browsers[] = {"msedge", "chrome", "chromium"};
#else
  const char *browsers[] = {
      "google-chrome", "chromium", "chromium-browser", "microsoft-edge"};
#endif
  for (const char *browser : browsers) {
    if (!command_exists(browser)) {
      continue;
    }
    std::ostringstream cmd;
    cmd << browser << " --headless --disable-gpu --no-pdf-header-footer "
        << "--print-to-pdf=" << pdf_q << " " << html_q;
    auto attempt = run_command(cmd.str(), browser);
    if (attempt.ok && std::filesystem::exists(pdf_path)) {
      return attempt;
    }
  }

  if (command_exists("wkhtmltopdf")) {
    std::ostringstream cmd;
    cmd << "wkhtmltopdf " << html_q << " " << pdf_q;
    auto attempt = run_command(cmd.str(), "wkhtmltopdf");
    if (attempt.ok && std::filesystem::exists(pdf_path)) {
      return attempt;
    }
  }

  /* Caller opens the system print dialog (Save as PDF) when no CLI tool is available. */
  result.error.clear();
  return result;
}

} // namespace stickinthemd
