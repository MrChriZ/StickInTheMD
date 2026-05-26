#include <doctest/doctest.h>

#include "stickinthemd/html_builder.hpp"
#include "stickinthemd/pdf_exporter.hpp"

#include <fstream>

TEST_CASE("PdfExporter falls back without external tools") {
  const auto html_path =
      std::filesystem::temp_directory_path() / "stickinthemd_export_test.html";
  const auto pdf_path =
      std::filesystem::temp_directory_path() / "stickinthemd_export_test.pdf";

  stickinthemd::HtmlBuilder html;
  std::ofstream out(html_path, std::ios::binary);
  out << html.print_page("<p>Export</p>", "Export");
  out.close();

  stickinthemd::PdfExporter exporter;
  const auto result = exporter.export_html_file(html_path, pdf_path);
  if (!result.ok) {
    CHECK(result.error.empty());
  } else {
    CHECK(std::filesystem::exists(pdf_path));
    std::filesystem::remove(pdf_path);
  }
  std::filesystem::remove(html_path);
}
