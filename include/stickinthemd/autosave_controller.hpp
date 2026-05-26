#pragma once

#include <chrono>

namespace stickinthemd {

class AutosaveController {
public:
  void set_enabled(bool enabled) { enabled_ = enabled; }
  bool enabled() const { return enabled_; }

  void set_interval(std::chrono::milliseconds interval) {
    interval_ = interval;
  }
  std::chrono::milliseconds interval() const { return interval_; }

  bool should_autosave(
      bool document_dirty, bool has_path,
      std::chrono::steady_clock::time_point last_edit,
      std::chrono::steady_clock::time_point last_autosave,
      std::chrono::steady_clock::time_point now) const;

private:
  bool enabled_ = true;
  std::chrono::milliseconds interval_{2000};
};

} // namespace stickinthemd
