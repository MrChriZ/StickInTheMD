#pragma once

#include <chrono>
#include <filesystem>
#include <string>

namespace stuckinthemd {

struct ExternalFileChange {
  // Avoid "None" — X11 headers define None as a macro on Linux.
  enum class Kind { NoChange, AutoReloaded, NeedsReloadConfirmation };

  Kind kind = Kind::NoChange;
  std::string disk_content;
  std::filesystem::file_time_type mtime{};
};

} // namespace stuckinthemd
