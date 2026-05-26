#include <doctest/doctest.h>

#include "stickinthemd/document_store.hpp"

#include <filesystem>

TEST_CASE("DocumentStore round-trips file content") {
  const auto path =
      std::filesystem::temp_directory_path() / "stickinthemd_test_doc.md";
  std::filesystem::remove(path);

  stickinthemd::DocumentStore store;
  const auto saved = store.save(path, "# Sample\n\nBody");
  REQUIRE(saved.ok);

  const auto loaded = store.load(path);
  REQUIRE(loaded.ok);
  CHECK(loaded.content == "# Sample\n\nBody");

  std::filesystem::remove(path);
}

TEST_CASE("DocumentStore reports missing files") {
  stickinthemd::DocumentStore store;
  const auto loaded = store.load(std::filesystem::path{"missing-file.md"});
  CHECK_FALSE(loaded.ok);
  CHECK_FALSE(loaded.error.empty());
}
