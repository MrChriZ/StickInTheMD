#pragma once

#include "stuckinthemd/color_theme.hpp"
#include "stuckinthemd/view_mode.hpp"

#include <chrono>

namespace stuckinthemd {

struct AppSettings {
  bool autosave_enabled = true;
  std::chrono::milliseconds autosave_interval{2000};
  ViewMode view_mode = ViewMode::Split;
  bool distraction_free = false;
  ColorTheme color_theme = ColorTheme::System;
};

} // namespace stuckinthemd
