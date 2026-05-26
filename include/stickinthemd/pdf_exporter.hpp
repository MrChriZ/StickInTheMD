#pragma once

#include <filesystem>
#include <string>

namespace stickinthemd {

struct PdfExportResult {
  bool ok = false;
  std::string method;
  std::string error;
};

class PdfExporter {
public:
  PdfExportResult export_html_file(const std::filesystem::path &html_path,
                                   const std::filesystem::path &pdf_path) const;
};

} // namespace stickinthemd
