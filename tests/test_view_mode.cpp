#include <doctest/doctest.h>

#include "stuckinthemd/view_mode.hpp"

TEST_CASE("view_mode_from_string parses layout names") {
  CHECK(stuckinthemd::view_mode_from_string("split") == stuckinthemd::ViewMode::Split);
  CHECK(stuckinthemd::view_mode_from_string("editor") == stuckinthemd::ViewMode::Editor);
  CHECK(stuckinthemd::view_mode_from_string("preview") == stuckinthemd::ViewMode::Preview);
  CHECK(stuckinthemd::view_mode_from_string("unknown") == stuckinthemd::ViewMode::Split);
}

TEST_CASE("cycle_view_mode rotates split editor preview") {
  CHECK(stuckinthemd::cycle_view_mode(stuckinthemd::ViewMode::Split) ==
        stuckinthemd::ViewMode::Editor);
  CHECK(stuckinthemd::cycle_view_mode(stuckinthemd::ViewMode::Editor) ==
        stuckinthemd::ViewMode::Preview);
  CHECK(stuckinthemd::cycle_view_mode(stuckinthemd::ViewMode::Preview) ==
        stuckinthemd::ViewMode::Split);
}
