#pragma once

#ifndef STUCKINTHEMD_VERSION
#define STUCKINTHEMD_VERSION "unknown"
#endif

namespace stuckinthemd {

inline constexpr const char *app_version() { return STUCKINTHEMD_VERSION; }

} // namespace stuckinthemd
