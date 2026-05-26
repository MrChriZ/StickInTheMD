#include <doctest/doctest.h>

#include "stuckinthemd/json_util.hpp"

TEST_CASE("json_escape handles quotes and newlines") {
  const auto json = stuckinthemd::json_escape("line\n\"quote\"");
  CHECK(json.find("\\n") != std::string::npos);
  CHECK(json.find("\\\"") != std::string::npos);
}

TEST_CASE("parse_json_string reads escaped content") {
  const auto value = stuckinthemd::parse_json_string(R"("hello\nworld")");
  CHECK(value == "hello\nworld");
}

TEST_CASE("bind_first_arg decodes webview JSON param arrays") {
  CHECK(stuckinthemd::bind_first_arg(R"(["# Title\n\nBody"])") == "# Title\n\nBody");
  CHECK(stuckinthemd::bind_first_arg(R"(["split"])") == "split");
  CHECK(stuckinthemd::bind_first_arg("# Hi") == "# Hi");
}

TEST_CASE("markdown_from_bind_request accepts JSON or raw text") {
  CHECK(stuckinthemd::markdown_from_bind_request(R"(["# Hi"])") == "# Hi");
}

TEST_CASE("bind_first_arg preserves quotes in decoded markdown") {
  const std::string md = "Choose \"Save as PDF\" here.";
  CHECK(stuckinthemd::bind_first_arg(md) == md);
}
