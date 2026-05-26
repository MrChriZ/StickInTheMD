#include <doctest/doctest.h>

#include "stuckinthemd/text_normalize.hpp"

TEST_CASE("normalize_document_text fixes literal escape sequences") {
  const std::string broken = R"("# Hello\n\nLine two")";
  const auto fixed = stuckinthemd::normalize_document_text(broken);
  CHECK(fixed.find('\n') != std::string::npos);
  CHECK(fixed.find("# Hello") == 0);
  CHECK(fixed.find("\\n") == std::string::npos);
}

TEST_CASE("normalize_document_text leaves normal markdown unchanged") {
  const std::string md = "# Title\n\nParagraph with \"quotes\".";
  CHECK(stuckinthemd::normalize_document_text(md) == md);
}
