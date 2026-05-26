#include "stickinthemd/color_theme.hpp"

namespace stickinthemd {

ColorTheme color_theme_from_string(std::string_view name) {
  if (name == "light") {
    return ColorTheme::Light;
  }
  if (name == "dark") {
    return ColorTheme::Dark;
  }
  return ColorTheme::System;
}

bool color_theme_is_dark(ColorTheme theme, bool system_prefers_dark) {
  switch (theme) {
  case ColorTheme::Light:
    return false;
  case ColorTheme::Dark:
    return true;
  case ColorTheme::System:
    return system_prefers_dark;
  }
  return false;
}

} // namespace stickinthemd
