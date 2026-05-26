#include "stickinthemd/view_mode.hpp"

namespace stickinthemd {

ViewMode view_mode_from_string(std::string_view name) {
  if (name == "editor") {
    return ViewMode::Editor;
  }
  if (name == "preview") {
    return ViewMode::Preview;
  }
  return ViewMode::Split;
}

ViewMode cycle_view_mode(ViewMode current) {
  switch (current) {
  case ViewMode::Split:
    return ViewMode::Editor;
  case ViewMode::Editor:
    return ViewMode::Preview;
  case ViewMode::Preview:
    return ViewMode::Split;
  }
  return ViewMode::Split;
}

} // namespace stickinthemd
