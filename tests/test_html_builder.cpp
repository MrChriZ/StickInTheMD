#include <doctest/doctest.h>

#include "stickinthemd/html_builder.hpp"

TEST_CASE("HtmlBuilder wraps preview document") {
  stickinthemd::HtmlBuilder builder;
  const auto page = builder.preview_page("<p>Hi</p>");
  CHECK(page.find("<!DOCTYPE html>") != std::string::npos);
  CHECK(page.find("<p>Hi</p>") != std::string::npos);
  CHECK(page.find("<style>") != std::string::npos);
}

TEST_CASE("HtmlBuilder includes title in print page") {
  stickinthemd::HtmlBuilder builder;
  const auto page = builder.print_page("<p>Hi</p>", "Notes");
  CHECK(page.find("<title>Notes</title>") != std::string::npos);
}

TEST_CASE("HtmlBuilder dark preview sets data-theme") {
  stickinthemd::HtmlBuilder builder;
  const auto page = builder.preview_page("<p>Hi</p>", true);
  CHECK(page.find("data-theme=\"dark\"") != std::string::npos);
}
