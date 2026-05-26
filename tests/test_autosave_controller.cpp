#include <doctest/doctest.h>

#include "stickinthemd/autosave_controller.hpp"

using steady_clock = std::chrono::steady_clock;

TEST_CASE("AutosaveController waits for interval after edits") {
  stickinthemd::AutosaveController autosave;
  autosave.set_interval(std::chrono::milliseconds(1000));

  const auto t0 = steady_clock::now();
  const auto last_edit = t0 + std::chrono::milliseconds(100);
  const auto last_save = t0;

  CHECK_FALSE(autosave.should_autosave(true, true, last_edit, last_save, t0 + std::chrono::milliseconds(500)));
  CHECK(autosave.should_autosave(true, true, last_edit, last_save, t0 + std::chrono::milliseconds(1200)));
}

TEST_CASE("AutosaveController requires path and dirty flag") {
  stickinthemd::AutosaveController autosave;
  const auto now = steady_clock::now();
  CHECK_FALSE(autosave.should_autosave(false, true, now, now, now + std::chrono::seconds(5)));
  CHECK_FALSE(autosave.should_autosave(true, false, now, now, now + std::chrono::seconds(5)));
}
