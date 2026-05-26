#pragma once

#include <chrono>
#include <filesystem>
#include <optional>
#include <string>

namespace stickinthemd {

class Document {
public:
  Document() = default;
  explicit Document(std::filesystem::path path, std::string content);

  const std::optional<std::filesystem::path> &path() const { return path_; }
  const std::string &content() const { return content_; }
  bool is_dirty() const { return dirty_; }
  bool has_path() const { return path_.has_value(); }

  void set_content(std::string content);
  void set_path(std::filesystem::path path);
  void clear_path();
  void mark_clean();
  void mark_dirty();

  std::chrono::steady_clock::time_point last_edit_time() const {
    return last_edit_;
  }

private:
  std::optional<std::filesystem::path> path_;
  std::string content_;
  bool dirty_ = false;
  std::chrono::steady_clock::time_point last_edit_ =
      std::chrono::steady_clock::now();
};

} // namespace stickinthemd
