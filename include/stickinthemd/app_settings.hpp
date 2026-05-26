#pragma once

#include "stickinthemd/color_theme.hpp"
#include "stickinthemd/view_mode.hpp"

#include <chrono>

namespace stickinthemd {

struct AppSettings {
  bool autosave_enabled = true;
  std::chrono::milliseconds autosave_interval{2000};
  ViewMode view_mode = ViewMode::Split;
  bool distraction_free = false;
  ColorTheme color_theme = ColorTheme::System;
};

} // namespace stickinthemd
