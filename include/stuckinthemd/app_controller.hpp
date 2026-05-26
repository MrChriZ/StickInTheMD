#pragma once

#include "stuckinthemd/app_settings.hpp"
#include "stuckinthemd/autosave_controller.hpp"
#include "stuckinthemd/document.hpp"
#include "stuckinthemd/external_file_change.hpp"
#include "stuckinthemd/document_store.hpp"
#include "stuckinthemd/html_builder.hpp"
#include "stuckinthemd/markdown_renderer.hpp"

#include <chrono>
#include <filesystem>
#include <optional>
#include <string>

namespace stuckinthemd {

struct OpenResult {
  bool ok = false;
  std::string error;
  std::optional<std::filesystem::path> path;
};

struct SaveResultInfo {
  bool ok = false;
  std::string error;
  std::optional<std::filesystem::path> path;
};

class AppController {
public:
  AppController();

  const Document &document() const { return document_; }
  const AppSettings &settings() const { return settings_; }
  AppSettings &settings() { return settings_; }

  std::string preview_html() const;
  std::string preview_html_for(const std::string &markdown,
                               bool dark_preview = false) const;
  std::string print_html() const;
  std::string render_body(const std::string &markdown) const;

  void new_document();
  OpenResult open_path(const std::filesystem::path &path);
  OpenResult open_dropped(const std::filesystem::path &path_hint,
                          const std::string &content);
  SaveResultInfo save();
  SaveResultInfo save_as_path(const std::filesystem::path &path);
  void update_content(const std::string &content);

  bool should_autosave(std::chrono::steady_clock::time_point now) const;
  SaveResultInfo perform_autosave();
  void note_autosave(std::chrono::steady_clock::time_point now);

  /** Re-read last-write time from disk (after open, save, or reload). */
  void sync_disk_state();
  void reload_from_disk(const std::string &content);
  ExternalFileChange detect_external_file_change();
  void acknowledge_external_revision(
      const std::filesystem::file_time_type &mtime);

private:
  Document document_;
  DocumentStore store_;
  MarkdownRenderer renderer_;
  HtmlBuilder html_;
  AppSettings settings_;
  std::chrono::steady_clock::time_point last_autosave_ =
      std::chrono::steady_clock::now();
  std::optional<std::filesystem::file_time_type> disk_mtime_;
  std::chrono::steady_clock::time_point last_own_write_ =
      std::chrono::steady_clock::time_point::min();
};

} // namespace stuckinthemd
