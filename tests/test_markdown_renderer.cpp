#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "stuckinthemd/markdown_renderer.hpp"

TEST_CASE("MarkdownRenderer converts headings and emphasis") {
  stuckinthemd::MarkdownRenderer renderer;
  const std::string html = renderer.to_html_body("# Title\n\n*emphasis*");
  CHECK(html.find("<h1>") != std::string::npos);
  CHECK(html.find("Title") != std::string::npos);
  CHECK(html.find("<em>") != std::string::npos);
}

TEST_CASE("MarkdownRenderer supports tables") {
  stuckinthemd::MarkdownRenderer renderer;
  const std::string md = "| A | B |\n|---|---|\n| 1 | 2 |";
  const std::string html = renderer.to_html_body(md);
  CHECK(html.find("<table>") != std::string::npos);
}
