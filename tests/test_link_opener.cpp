#include <doctest/doctest.h>

#include "stuckinthemd/html_builder.hpp"
#include "stuckinthemd/link_opener.hpp"

#include <filesystem>
#include <fstream>

TEST_CASE("is_external_href detects web and mail links") {
  CHECK(stuckinthemd::is_external_href("https://example.com"));
  CHECK(stuckinthemd::is_external_href("http://example.com/page"));
  CHECK(stuckinthemd::is_external_href("mailto:user@example.com"));
  CHECK(stuckinthemd::is_external_href("//cdn.example.com/x"));
  CHECK_FALSE(stuckinthemd::is_external_href("notes.md"));
  CHECK_FALSE(stuckinthemd::is_external_href("#section"));
}

TEST_CASE("is_markdown_file recognizes markdown extensions") {
  CHECK(stuckinthemd::is_markdown_file(std::filesystem::path{"a.md"}));
  CHECK(stuckinthemd::is_markdown_file(std::filesystem::path{"b.markdown"}));
  CHECK_FALSE(stuckinthemd::is_markdown_file(std::filesystem::path{"c.png"}));
  CHECK_FALSE(stuckinthemd::is_markdown_file(std::filesystem::path{"d.pdf"}));
}

TEST_CASE("resolve_preview_link maps relative markdown beside document") {
  const auto root =
      std::filesystem::temp_directory_path() / "stuckinthemd_link_resolve";
  std::filesystem::remove_all(root);
  std::filesystem::create_directories(root);

  const auto doc = root / "README.md";
  {
    std::ofstream out(doc);
    out << "# Doc";
  }

  const auto resolved =
      stuckinthemd::resolve_preview_link(doc, "CONTRIBUTING.md");
  REQUIRE(resolved.has_value());
  CHECK(resolved->action == stuckinthemd::PreviewLinkAction::OpenInApp);
  CHECK(resolved->target.find("CONTRIBUTING.md") != std::string::npos);

  std::filesystem::remove_all(root);
}

TEST_CASE("resolve_preview_link sends https URLs externally") {
  const auto resolved =
      stuckinthemd::resolve_preview_link(std::filesystem::path{"README.md"},
                                         "https://github.com/example");
  REQUIRE(resolved.has_value());
  CHECK(resolved->action == stuckinthemd::PreviewLinkAction::OpenExternally);
  CHECK(resolved->target == "https://github.com/example");
}

TEST_CASE("resolve_preview_link opens local non-markdown files externally") {
  const auto root =
      std::filesystem::temp_directory_path() / "stuckinthemd_link_png";
  std::filesystem::remove_all(root);
  std::filesystem::create_directories(root);

  const auto doc = root / "README.md";
  {
    std::ofstream out(doc);
    out << "doc";
  }
  {
    std::ofstream png(root / "shot.png");
    png << "x";
  }

  const auto resolved =
      stuckinthemd::resolve_preview_link(doc, "shot.png");
  REQUIRE(resolved.has_value());
  CHECK(resolved->action == stuckinthemd::PreviewLinkAction::OpenExternally);
  CHECK(resolved->target.find("shot.png") != std::string::npos);

  std::filesystem::remove_all(root);
}

TEST_CASE("HtmlBuilder preview includes link click bridge script") {
  stuckinthemd::HtmlBuilder builder;
  const auto page = builder.preview_page("<p><a href=\"x\">link</a></p>");
  CHECK(page.find("stuck-preview-link") != std::string::npos);
  CHECK(page.find("postMessage") != std::string::npos);
}
