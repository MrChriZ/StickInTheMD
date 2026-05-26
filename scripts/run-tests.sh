#!/usr/bin/env bash
# Run the same configure / build / test steps as CI (Linux/macOS).
set -euo pipefail

root="$(cd "$(dirname "$0")/.." && pwd)"
cd "$root"

if [[ ! -d third_party/md4c/.git ]]; then
  bash scripts/bootstrap-third-party.sh
fi

build_type="${CMAKE_BUILD_TYPE:-Release}"
generator=()
extra_ctest=()

if [[ "$(uname -s)" == "Linux" ]]; then
  generator=(-G Ninja)
fi

if [[ "$(uname -s)" == "Darwin" ]]; then
  bash scripts/build-macos-icon.sh
  extra_ctest=(-C Release)
fi

cmake -S . -B build "${generator[@]}" \
  -DCMAKE_BUILD_TYPE="$build_type" \
  -DSTUCKINTHEMD_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build "${extra_ctest[@]}" --output-on-failure

echo "All tests passed."
