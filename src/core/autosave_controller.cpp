#include "stickinthemd/autosave_controller.hpp"

namespace stickinthemd {

bool AutosaveController::should_autosave(
    bool document_dirty, bool has_path,
    std::chrono::steady_clock::time_point last_edit,
    std::chrono::steady_clock::time_point last_autosave,
    std::chrono::steady_clock::time_point now) const {
  if (!enabled_ || !document_dirty || !has_path) {
    return false;
  }
  if (last_edit <= last_autosave) {
    return false;
  }
  return (now - last_autosave) >= interval_;
}

} // namespace stickinthemd
