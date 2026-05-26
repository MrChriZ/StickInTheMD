#include "stickinthemd/app_controller.hpp"

#include "stickinthemd/text_normalize.hpp"

#include <system_error>

namespace stickinthemd {

namespace {

bool is_existing_file_path(const std::filesystem::path &path) {
  if (path.empty()) {
    return false;
  }
  const auto str = path.string();
  if (!path.is_absolute() && str.find_first_of("/\\") == std::string::npos) {
    return false;
  }
  std::error_code ec;
  return std::filesystem::is_regular_file(path, ec);
}

} // namespace

AppController::AppController() { new_document(); }

std::string AppController::render_body(const std::string &markdown) const {
  return renderer_.to_html_body(markdown);
}

std::string AppController::preview_html() const {
  return preview_html_for(document_.content());
}

std::string AppController::preview_html_for(const std::string &markdown,
                                            const bool dark_preview) const {
  return html_.preview_page(render_body(markdown), dark_preview);
}

std::string AppController::print_html() const {
  std::string title = "Untitled";
  if (document_.has_path()) {
    title = document_.path()->filename().string();
  }
  return html_.print_page(render_body(document_.content()), title);
}

void AppController::new_document() {
  document_ = Document{};
  document_.set_content("");
  document_.mark_clean();
}

OpenResult AppController::open_path(const std::filesystem::path &path) {
  OpenResult result;
  const auto loaded = store_.load(path);
  if (!loaded.ok) {
    result.error = loaded.error;
    return result;
  }
  document_ = Document{path, loaded.content};
  document_.mark_clean();
  result.ok = true;
  result.path = path;
  return result;
}

OpenResult AppController::open_dropped(const std::filesystem::path &path_hint,
                                       const std::string &content) {
  if (is_existing_file_path(path_hint)) {
    return open_path(path_hint);
  }

  OpenResult result;
  document_ = Document{};
  document_.set_content(normalize_document_text(content));
  if (!path_hint.empty()) {
    document_.set_path(path_hint);
  }
  document_.mark_clean();
  result.ok = true;
  if (!path_hint.empty()) {
    result.path = path_hint;
  }
  return result;
}

SaveResultInfo AppController::save() {
  SaveResultInfo result;
  if (!document_.has_path()) {
    result.error = "No file path set.";
    return result;
  }
  return save_as_path(*document_.path());
}

SaveResultInfo AppController::save_as_path(const std::filesystem::path &path) {
  SaveResultInfo result;
  const auto saved = store_.save(path, document_.content());
  if (!saved.ok) {
    result.error = saved.error;
    return result;
  }
  document_.set_path(path);
  document_.mark_clean();
  result.ok = true;
  result.path = path;
  return result;
}

void AppController::update_content(const std::string &content) {
  document_.set_content(content);
}

bool AppController::should_autosave(
    std::chrono::steady_clock::time_point now) const {
  AutosaveController timing;
  timing.set_enabled(settings_.autosave_enabled);
  timing.set_interval(settings_.autosave_interval);
  return timing.should_autosave(document_.is_dirty(), document_.has_path(),
                                document_.last_edit_time(), last_autosave_, now);
}

SaveResultInfo AppController::perform_autosave() {
  return save();
}

void AppController::note_autosave(std::chrono::steady_clock::time_point now) {
  last_autosave_ = now;
}

} // namespace stickinthemd
