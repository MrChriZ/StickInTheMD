#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

clone_if_missing() {
  local url="$1"
  local dir="$2"
  local branch="${3:-}"
  if [[ -d "$dir/.git" ]]; then
    return 0
  fi
  echo "Cloning $url -> $dir"
  if [[ -n "$branch" ]]; then
    git clone --depth 1 --branch "$branch" "$url" "$dir"
  else
    git clone --depth 1 "$url" "$dir"
  fi
}

clone_if_missing "https://github.com/mity/md4c.git" "third_party/md4c"
clone_if_missing "https://github.com/doctest/doctest.git" "third_party/doctest" "v2.4.12"
clone_if_missing "https://github.com/webview/webview.git" "third_party/webview"

TFD_DIR="third_party/tinyfiledialogs"
if [[ ! -f "$TFD_DIR/tinyfiledialogs.c" ]]; then
  mkdir -p "$TFD_DIR"
  curl -fsSL -o "$TFD_DIR/tinyfiledialogs.h" \
    "https://raw.githubusercontent.com/native-toolkit/libtinyfiledialogs/master/tinyfiledialogs.h"
  curl -fsSL -o "$TFD_DIR/tinyfiledialogs.c" \
    "https://raw.githubusercontent.com/native-toolkit/libtinyfiledialogs/master/tinyfiledialogs.c"
fi

echo "Third-party dependencies are ready."
