#pragma once

#include <string>
#include <string_view>

namespace stuckinthemd {

enum class ColorTheme { System, Light, Dark };

constexpr std::string_view color_theme_name(ColorTheme theme) {
  switch (theme) {
  case ColorTheme::System:
    return "system";
  case ColorTheme::Light:
    return "light";
  case ColorTheme::Dark:
    return "dark";
  }
  return "system";
}

ColorTheme color_theme_from_string(std::string_view name);

/** Resolved appearance for preview/print chrome (system treated as light). */
bool color_theme_is_dark(ColorTheme theme, bool system_prefers_dark);

} // namespace stuckinthemd
