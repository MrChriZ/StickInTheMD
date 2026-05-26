#include "stuckinthemd/document.hpp"

namespace stuckinthemd {

Document::Document(std::filesystem::path path, std::string content)
    : path_(std::move(path)), content_(std::move(content)), dirty_(false) {}

void Document::set_content(std::string content) {
  if (content_ != content) {
    content_ = std::move(content);
    dirty_ = true;
    last_edit_ = std::chrono::steady_clock::now();
  }
}

void Document::set_path(std::filesystem::path path) { path_ = std::move(path); }

void Document::clear_path() { path_.reset(); }

void Document::mark_clean() { dirty_ = false; }

void Document::mark_dirty() {
  dirty_ = true;
  last_edit_ = std::chrono::steady_clock::now();
}

} // namespace stuckinthemd
