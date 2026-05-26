#include <doctest/doctest.h>

#include "stuckinthemd/app_controller.hpp"

#include <atomic>
#include <filesystem>
#include <fstream>
#include <string>
#include <thread>

namespace {

void write_bytes(const std::filesystem::path &path, const std::string &content) {
  std::ofstream out(path, std::ios::binary | std::ios::trunc);
  REQUIRE(out.good());
  out.write(content.data(), static_cast<std::streamsize>(content.size()));
  REQUIRE(out.good());
}

void sleep_for_mtime() {
  std::this_thread::sleep_for(std::chrono::milliseconds(1100));
}

} // namespace

TEST_CASE("AppController renders preview html from markdown") {
  stuckinthemd::AppController controller;
  controller.update_content("# Hello");
  const auto html = controller.preview_html();
  CHECK(html.find("<h1>") != std::string::npos);
  CHECK(html.find("Hello") != std::string::npos);
}

TEST_CASE("AppController preview_html_for uses supplied markdown") {
  stuckinthemd::AppController controller;
  const auto html = controller.preview_html_for("## Title");
  CHECK(html.find("<h2>") != std::string::npos);
  CHECK(html.find("Title") != std::string::npos);
}

TEST_CASE("AppController saves and reloads documents") {
  const auto path =
      std::filesystem::temp_directory_path() / "stuckinthemd_app_test.md";
  std::filesystem::remove(path);

  stuckinthemd::AppController controller;
  controller.update_content("content");
  const auto saved = controller.save_as_path(path);
  REQUIRE(saved.ok);
  CHECK_FALSE(controller.document().is_dirty());

  stuckinthemd::AppController reloaded;
  const auto opened = reloaded.open_path(path);
  REQUIRE(opened.ok);
  CHECK(reloaded.document().content() == "content");

  std::filesystem::remove(path);
}

TEST_CASE("AppController open_dropped uses content when path is only a filename") {
  stuckinthemd::AppController controller;
  const auto opened =
      controller.open_dropped("notes.md", "# Dropped\n\nBody");
  REQUIRE(opened.ok);
  CHECK(controller.document().content().find("# Dropped") != std::string::npos);
  CHECK(controller.document().path()->filename() == "notes.md");
}

TEST_CASE("AppController default view settings") {
  stuckinthemd::AppController controller;
  CHECK(controller.settings().view_mode == stuckinthemd::ViewMode::Split);
  CHECK_FALSE(controller.settings().distraction_free);
}

TEST_CASE("AppController autosave triggers only when dirty and timed") {
  stuckinthemd::AppController controller;
  controller.settings().autosave_interval = std::chrono::milliseconds(10);
  controller.update_content("draft");

  const auto path =
      std::filesystem::temp_directory_path() / "stuckinthemd_autosave.md";
  std::filesystem::remove(path);
  controller.save_as_path(path);
  controller.update_content("updated draft");

  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  const auto now = std::chrono::steady_clock::now();
  CHECK(controller.should_autosave(now));
}

TEST_CASE("AppController auto-reloads when disk changes and editor is clean") {
  const auto path =
      std::filesystem::temp_directory_path() / "stuckinthemd_external_watch.md";
  std::filesystem::remove(path);

  stuckinthemd::AppController controller;
  controller.update_content("version one");
  controller.save_as_path(path);
  std::this_thread::sleep_for(std::chrono::milliseconds(1100));

  {
    std::ofstream out(path, std::ios::trunc);
    out << "version two";
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(1100));

  const auto change = controller.detect_external_file_change();
  CHECK(change.kind == stuckinthemd::ExternalFileChange::Kind::AutoReloaded);
  CHECK(controller.document().content() == "version two");
  CHECK_FALSE(controller.document().is_dirty());

  std::filesystem::remove(path);
}

TEST_CASE("AppController prompts when disk changes and editor is dirty") {
  const auto path =
      std::filesystem::temp_directory_path() / "stuckinthemd_external_dirty.md";
  std::filesystem::remove(path);

  stuckinthemd::AppController controller;
  controller.update_content("saved baseline");
  controller.save_as_path(path);
  controller.update_content("local edits");
  std::this_thread::sleep_for(std::chrono::milliseconds(1100));

  {
    std::ofstream out(path, std::ios::trunc);
    out << "on disk";
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(1100));

  const auto change = controller.detect_external_file_change();
  CHECK(change.kind ==
        stuckinthemd::ExternalFileChange::Kind::NeedsReloadConfirmation);
  CHECK(change.disk_content == "on disk");
  CHECK(controller.document().content() == "local edits");
  CHECK(controller.document().is_dirty());

  const auto again = controller.detect_external_file_change();
  CHECK(again.kind == stuckinthemd::ExternalFileChange::Kind::NoChange);

  std::filesystem::remove(path);
}

TEST_CASE("AppController detect ignores own save within grace period") {
  const auto path =
      std::filesystem::temp_directory_path() / "stuckinthemd_own_save_grace.md";
  std::filesystem::remove(path);

  stuckinthemd::AppController controller;
  controller.update_content("saved once");
  const auto saved = controller.save_as_path(path);
  REQUIRE(saved.ok);

  const auto change = controller.detect_external_file_change();
  CHECK(change.kind == stuckinthemd::ExternalFileChange::Kind::NoChange);
  CHECK(controller.document().content() == "saved once");

  std::filesystem::remove(path);
}

TEST_CASE("AppController detect ignores mtime-only change when content matches") {
  const auto path =
      std::filesystem::temp_directory_path() / "stuckinthemd_mtime_only.md";
  std::filesystem::remove(path);

  stuckinthemd::AppController controller;
  controller.update_content("stable body");
  REQUIRE(controller.save_as_path(path).ok);
  sleep_for_mtime();

  write_bytes(path, "stable body");
  sleep_for_mtime();

  const auto change = controller.detect_external_file_change();
  CHECK(change.kind == stuckinthemd::ExternalFileChange::Kind::NoChange);
  CHECK(controller.document().content() == "stable body");
  CHECK_FALSE(controller.document().is_dirty());

  std::filesystem::remove(path);
}

TEST_CASE("AppController detect matches disk after literal escape normalization") {
  const auto path =
      std::filesystem::temp_directory_path() / "stuckinthemd_literal_newlines.md";
  std::filesystem::remove(path);

  const std::string literal_on_disk = "# Hello\\n\\nLine two";
  write_bytes(path, literal_on_disk);

  stuckinthemd::AppController controller;
  const auto opened = controller.open_path(path);
  REQUIRE(opened.ok);
  CHECK(controller.document().content().find('\n') != std::string::npos);
  sleep_for_mtime();

  const auto change = controller.detect_external_file_change();
  CHECK(change.kind == stuckinthemd::ExternalFileChange::Kind::NoChange);

  std::filesystem::remove(path);
}

TEST_CASE("AppController repeated detect after save does not auto-reload") {
  const auto path =
      std::filesystem::temp_directory_path() / "stuckinthemd_detect_stable.md";
  std::filesystem::remove(path);

  stuckinthemd::AppController controller;
  controller.update_content("no false reload");
  REQUIRE(controller.save_as_path(path).ok);
  sleep_for_mtime();

  for (int i = 0; i < 5; ++i) {
    const auto change = controller.detect_external_file_change();
    CHECK(change.kind == stuckinthemd::ExternalFileChange::Kind::NoChange);
  }
  CHECK(controller.document().content() == "no false reload");
  CHECK_FALSE(controller.document().is_dirty());

  std::filesystem::remove(path);
}

TEST_CASE("AppController reload_from_disk is idempotent and stays clean") {
  const auto path =
      std::filesystem::temp_directory_path() / "stuckinthemd_reload_idempotent.md";
  std::filesystem::remove(path);

  stuckinthemd::AppController controller;
  controller.update_content("same bytes");
  REQUIRE(controller.save_as_path(path).ok);

  controller.reload_from_disk("same bytes");
  CHECK(controller.document().content() == "same bytes");
  CHECK_FALSE(controller.document().is_dirty());

  controller.reload_from_disk(controller.document().content());
  CHECK_FALSE(controller.document().is_dirty());

  std::filesystem::remove(path);
}

TEST_CASE("AppController concurrent save and detect stays consistent") {
  const auto path =
      std::filesystem::temp_directory_path() / "stuckinthemd_save_detect_race.md";
  std::filesystem::remove(path);

  stuckinthemd::AppController controller;
  controller.update_content("v0");
  REQUIRE(controller.save_as_path(path).ok);

  std::atomic<bool> stop{false};
  std::thread watcher([&] {
    while (!stop.load()) {
      controller.detect_external_file_change();
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  });

  for (int i = 1; i <= 25; ++i) {
    const std::string content = "version " + std::to_string(i);
    controller.update_content(content);
    const auto saved = controller.save();
    REQUIRE(saved.ok);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }

  stop = true;
  watcher.join();
  sleep_for_mtime();

  const auto change = controller.detect_external_file_change();
  CHECK(change.kind == stuckinthemd::ExternalFileChange::Kind::NoChange);
  CHECK(controller.document().content() == "version 25");
  CHECK_FALSE(controller.document().is_dirty());

  std::filesystem::remove(path);
}

TEST_CASE("AppController preview_html_for embeds README-style local images") {
  const auto root =
      std::filesystem::temp_directory_path() / "stuckinthemd_readme_preview";
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
    std::ofstream png(assets / "screenshot.png", std::ios::binary);
    png.write(reinterpret_cast<const char *>(k_png), sizeof(k_png));
  }

  const auto readme = root / "README.md";
  write_bytes(readme, "![preview](assets/screenshot.png)\n");

  stuckinthemd::AppController controller;
  const auto opened = controller.open_path(readme);
  REQUIRE(opened.ok);

  const auto html = controller.preview_html();
  CHECK(html.find("data:image/png;base64,") != std::string::npos);

  std::filesystem::remove_all(root);
}
