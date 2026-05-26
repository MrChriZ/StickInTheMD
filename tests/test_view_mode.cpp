#include <doctest/doctest.h>

#include "stickinthemd/view_mode.hpp"

TEST_CASE("view_mode_from_string parses layout names") {
  CHECK(stickinthemd::view_mode_from_string("split") == stickinthemd::ViewMode::Split);
  CHECK(stickinthemd::view_mode_from_string("editor") == stickinthemd::ViewMode::Editor);
  CHECK(stickinthemd::view_mode_from_string("preview") == stickinthemd::ViewMode::Preview);
  CHECK(stickinthemd::view_mode_from_string("unknown") == stickinthemd::ViewMode::Split);
}

TEST_CASE("cycle_view_mode rotates split editor preview") {
  CHECK(stickinthemd::cycle_view_mode(stickinthemd::ViewMode::Split) ==
        stickinthemd::ViewMode::Editor);
  CHECK(stickinthemd::cycle_view_mode(stickinthemd::ViewMode::Editor) ==
        stickinthemd::ViewMode::Preview);
  CHECK(stickinthemd::cycle_view_mode(stickinthemd::ViewMode::Preview) ==
        stickinthemd::ViewMode::Split);
}
