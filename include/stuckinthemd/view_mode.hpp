#pragma once

#include <string>
#include <string_view>

namespace stuckinthemd {

enum class ViewMode { Split, Editor, Preview };

constexpr std::string_view view_mode_name(ViewMode mode) {
  switch (mode) {
  case ViewMode::Split:
    return "split";
  case ViewMode::Editor:
    return "editor";
  case ViewMode::Preview:
    return "preview";
  }
  return "split";
}

ViewMode view_mode_from_string(std::string_view name);

ViewMode cycle_view_mode(ViewMode current);

} // namespace stuckinthemd
