#include "stickinthemd/file_dialogs.hpp"

#include <cstring>

extern "C" {
#include "tinyfiledialogs.h"
}

namespace stickinthemd {

void set_file_dialog_parent(void *native_window) {
#ifdef _WIN32
  tinyfd_winOwner = native_window;
#else
  (void)native_window;
#endif
}

namespace {

char *k_filters[] = {const_cast<char *>("*.md"),
                     const_cast<char *>("*.markdown"),
                     const_cast<char *>("*.txt"), const_cast<char *>("*.*")};
constexpr int k_filter_count = 4;

std::optional<std::filesystem::path> from_dialog(const char *path) {
  if (path == nullptr || path[0] == '\0') {
    return std::nullopt;
  }
  std::filesystem::path result(path);
  return result;
}

} // namespace

std::optional<std::filesystem::path> pick_open_markdown() {
  const char *path = tinyfd_openFileDialog("Open Markdown", "", k_filter_count,
                                           k_filters, nullptr, 0);
  return from_dialog(path);
}

std::optional<std::filesystem::path>
pick_save_markdown(const std::filesystem::path &suggested) {
  const std::string start = suggested.empty() ? "" : suggested.string();
  const char *path = tinyfd_saveFileDialog("Save Markdown", start.c_str(),
                                           k_filter_count, k_filters, nullptr);
  return from_dialog(path);
}

} // namespace stickinthemd
