#include <doctest/doctest.h>

#include "stickinthemd/document.hpp"

TEST_CASE("Document tracks dirty state on edits") {
  stickinthemd::Document doc;
  CHECK_FALSE(doc.is_dirty());
  doc.set_content("hello");
  CHECK(doc.is_dirty());
  CHECK(doc.content() == "hello");
  doc.mark_clean();
  CHECK_FALSE(doc.is_dirty());
}

TEST_CASE("Document stores optional path") {
  stickinthemd::Document doc{std::filesystem::path{"notes.md"}, "# Hi"};
  CHECK(doc.has_path());
  CHECK(doc.path()->filename() == "notes.md");
  doc.clear_path();
  CHECK_FALSE(doc.has_path());
}
