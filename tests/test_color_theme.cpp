#include <doctest/doctest.h>

#include "stuckinthemd/color_theme.hpp"

TEST_CASE("color_theme_from_string") {
  CHECK(stuckinthemd::color_theme_from_string("light") == stuckinthemd::ColorTheme::Light);
  CHECK(stuckinthemd::color_theme_from_string("dark") == stuckinthemd::ColorTheme::Dark);
  CHECK(stuckinthemd::color_theme_from_string("system") == stuckinthemd::ColorTheme::System);
  CHECK(stuckinthemd::color_theme_from_string("unknown") == stuckinthemd::ColorTheme::System);
}

TEST_CASE("color_theme_is_dark") {
  CHECK_FALSE(stuckinthemd::color_theme_is_dark(stuckinthemd::ColorTheme::Light, true));
  CHECK(stuckinthemd::color_theme_is_dark(stuckinthemd::ColorTheme::Dark, false));
  CHECK(stuckinthemd::color_theme_is_dark(stuckinthemd::ColorTheme::System, true));
  CHECK_FALSE(stuckinthemd::color_theme_is_dark(stuckinthemd::ColorTheme::System, false));
}
