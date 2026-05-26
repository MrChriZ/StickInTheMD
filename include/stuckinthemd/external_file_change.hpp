#pragma once

#include <chrono>
#include <filesystem>
#include <string>

namespace stuckinthemd {

struct ExternalFileChange {
  enum class Kind { None, AutoReloaded, NeedsReloadConfirmation };

  Kind kind = Kind::None;
  std::string disk_content;
  std::filesystem::file_time_type mtime{};
};

} // namespace stuckinthemd
