#include <doctest/doctest.h>

#include "stuckinthemd/html_builder.hpp"

#include <filesystem>
#include <fstream>

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

TEST_CASE("path_to_file_url produces a directory file URI") {
  const auto dir = std::filesystem::temp_directory_path();
  const auto url = stuckinthemd::path_to_file_url(dir);
  CHECK(url.find("file://") == 0);
  CHECK(url.back() == '/');
}

TEST_CASE("HtmlBuilder preview adds base href for resource directory") {
  const auto dir = std::filesystem::temp_directory_path();
  stuckinthemd::HtmlBuilder builder;
  const auto page =
      builder.preview_page("<img src=\"assets/x.png\" alt=\"\">", false, dir);
  const std::string expected =
      "<base href=\"" + stuckinthemd::path_to_file_url(dir) + "\">";
  CHECK(page.find(expected) != std::string::npos);
}

TEST_CASE("embed_local_images_for_preview inlines existing image files") {
  const auto root =
      std::filesystem::temp_directory_path() / "stuckinthemd_embed_img";
  const auto assets = root / "assets";
  std::filesystem::remove_all(root);
  std::filesystem::create_directories(assets);

  static const unsigned char k_png[] = {
      0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D,
      0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01,
      0x08, 0x06, 0x00, 0x00, 0x00, 0x1F, 0x15, 0xC4, 0x89, 0x00, 0x00, 0x00,
      0x0A, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9C, 0x63, 0x00, 0x01, 0x00, 0x00,
      0x05, 0x00, 0x01, 0x0D, 0x0A, 0x2D, 0xB4, 0x00, 0x00, 0x00, 0x00, 0x49,
      0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82};
  {
    std::ofstream png(assets / "x.png", std::ios::binary);
    png.write(reinterpret_cast<const char *>(k_png), sizeof(k_png));
  }

  const std::string body = R"(<img src="assets/x.png" alt="test">)";
  const auto embedded = stuckinthemd::embed_local_images_for_preview(body, root);
  CHECK(embedded.find("data:image/png;base64,") != std::string::npos);
  CHECK(embedded.find(R"(src="assets/x.png")") == std::string::npos);

  std::filesystem::remove_all(root);
}

TEST_CASE("embed_local_images_for_preview leaves remote and missing src unchanged") {
  const auto root =
      std::filesystem::temp_directory_path() / "stuckinthemd_embed_skip";
  std::filesystem::remove_all(root);
  std::filesystem::create_directories(root);

  const std::string body =
      R"(<img src="https://example.com/a.png"><img src="missing.png">)";
  const auto embedded = stuckinthemd::embed_local_images_for_preview(body, root);
  CHECK(embedded.find("https://example.com/a.png") != std::string::npos);
  CHECK(embedded.find(R"(src="missing.png")") != std::string::npos);

  std::filesystem::remove_all(root);
}

TEST_CASE("HtmlBuilder preview embeds local images for srcdoc preview") {
  const auto root =
      std::filesystem::temp_directory_path() / "stuckinthemd_preview_page";
  const auto assets = root / "assets";
  std::filesystem::remove_all(root);
  std::filesystem::create_directories(assets);

  static const unsigned char k_png[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A,
                                        0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48,
                                        0x44, 0x52, 0x00, 0x00, 0x00, 0x01, 0x00,
                                        0x00, 0x00, 0x01, 0x08, 0x06, 0x00, 0x00,
                                        0x00, 0x1F, 0x15, 0xC4, 0x89, 0x00, 0x00,
                                        0x00, 0x0A, 0x49, 0x44, 0x41, 0x54, 0x78,
                                        0x9C, 0x63, 0x00, 0x01, 0x00, 0x00, 0x05,
                                        0x00, 0x01, 0x0D, 0x0A, 0x2D, 0xB4, 0x00,
                                        0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44,
                                        0xAE, 0x42, 0x60, 0x82};
  {
    std::ofstream png(assets / "shot.png", std::ios::binary);
    png.write(reinterpret_cast<const char *>(k_png), sizeof(k_png));
  }

  stuckinthemd::HtmlBuilder builder;
  const auto page = builder.preview_page(
      R"(<img src="assets/shot.png" alt="screenshot">)", false, root);
  CHECK(page.find("data:image/png;base64,") != std::string::npos);

  std::filesystem::remove_all(root);
}
