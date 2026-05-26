#!/usr/bin/env bash
# Build macOS .icns from assets/icon/stickinthemd-*.png (requires macOS iconutil)
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
ICONSET="$ROOT/assets/icon/stickinthemd.iconset"
OUT="$ROOT/assets/icon/stickinthemd.icns"
ICON_SRC="$ROOT/assets/icon"

mkdir -p "$ICONSET"
copy() { cp "$ICON_SRC/stickinthemd-$1.png" "$ICONSET/icon_$2"; }

copy 16 16x16.png
copy 32 32x32.png
copy 32 16x16@2x.png
copy 128 128x128.png
copy 256 128x128@2x.png
copy 256 256x256.png
copy 512 256x256@2x.png

rm -f "$OUT"
iconutil -c icns "$ICONSET" -o "$OUT"
echo "Wrote $OUT"
