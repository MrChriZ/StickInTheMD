#include <doctest/doctest.h>

#include "stuckinthemd/html_builder.hpp"

#include <filesystem>

TEST_CASE("HtmlBuilder wraps preview document") {
  stuckinthemd::HtmlBuilder builder;
  const auto page = builder.preview_page("<p>Hi</p>");
  CHECK(page.find("<!DOCTYPE html>") != std::string::npos);
  CHECK(page.find("<p>Hi</p>") != std::string::npos);
  CHECK(page.find("<style>") != std::string::npos);
}

TEST_CASE("HtmlBuilder includes title in print page") {
  stuckinthemd::HtmlBuilder builder;
  const auto page = builder.print_page("<p>Hi</p>", "Notes");
  CHECK(page.find("<title>Notes</title>") != std::string::npos);
}

TEST_CASE("HtmlBuilder dark preview sets data-theme") {
  stuckinthemd::HtmlBuilder builder;
  const auto page = builder.preview_page("<p>Hi</p>", true);
  CHECK(page.find("data-theme=\"dark\"") != std::string::npos);
}

TEST_CASE("path_to_file_url encodes Windows paths") {
  const auto url = stuckinthemd::path_to_file_url(
      std::filesystem::path("C:/project"));
  CHECK(url.find("file:///C:/project") == 0);
  CHECK(url.back() == '/');
}

TEST_CASE("HtmlBuilder preview adds base href for resource directory") {
  stuckinthemd::HtmlBuilder builder;
  const auto page = builder.preview_page(
      "<img src=\"assets/x.png\" alt=\"\">", false,
      std::filesystem::path("C:/project"));
  CHECK(page.find("<base href=\"file:///C:/project/") != std::string::npos);
}
