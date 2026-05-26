#pragma once

#include <filesystem>
#include <optional>

namespace stuckinthemd {

/** Native top-level window handle (HWND on Windows). Modal dialogs use this as owner. */
void set_file_dialog_parent(void *native_window);

std::optional<std::filesystem::path> pick_open_markdown();
std::optional<std::filesystem::path> pick_save_markdown(
    const std::filesystem::path &suggested = {});

bool confirm_reload_externally_modified(const std::filesystem::path &path);

} // namespace stuckinthemd
