#pragma once

#ifndef STICKINTHEMD_VERSION
#define STICKINTHEMD_VERSION "unknown"
#endif

namespace stickinthemd {

inline constexpr const char *app_version() { return STICKINTHEMD_VERSION; }

} // namespace stickinthemd
