#include <doctest/doctest.h>

#include "stuckinthemd/app_controller.hpp"

#include <filesystem>
#include <fstream>
#include <thread>

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
  CHECK(again.kind == stuckinthemd::ExternalFileChange::Kind::None);

  std::filesystem::remove(path);
}
