#include <doctest/doctest.h>

#include "stickinthemd/color_theme.hpp"

TEST_CASE("color_theme_from_string") {
  CHECK(stickinthemd::color_theme_from_string("light") == stickinthemd::ColorTheme::Light);
  CHECK(stickinthemd::color_theme_from_string("dark") == stickinthemd::ColorTheme::Dark);
  CHECK(stickinthemd::color_theme_from_string("system") == stickinthemd::ColorTheme::System);
  CHECK(stickinthemd::color_theme_from_string("unknown") == stickinthemd::ColorTheme::System);
}

TEST_CASE("color_theme_is_dark") {
  CHECK_FALSE(stickinthemd::color_theme_is_dark(stickinthemd::ColorTheme::Light, true));
  CHECK(stickinthemd::color_theme_is_dark(stickinthemd::ColorTheme::Dark, false));
  CHECK(stickinthemd::color_theme_is_dark(stickinthemd::ColorTheme::System, true));
  CHECK_FALSE(stickinthemd::color_theme_is_dark(stickinthemd::ColorTheme::System, false));
}
