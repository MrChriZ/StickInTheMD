#!/usr/bin/env python3
"""Regenerate stuckinthemd.ico and PNG sizes from assets/icon/stuckinthemd-256.png."""
from __future__ import annotations

from pathlib import Path

try:
    from PIL import Image
except ImportError:
    raise SystemExit("Install Pillow: pip install Pillow")

ROOT = Path(__file__).resolve().parents[1]
ICON_DIR = ROOT / "assets" / "icon"
SRC = ICON_DIR / "stuckinthemd-256.png"

if not SRC.exists():
    raise SystemExit(f"Missing source icon: {SRC}")

img = Image.open(SRC).convert("RGBA")
for size in (16, 32, 48, 64, 128, 256, 512):
    img.resize((size, size), Image.Resampling.LANCZOS).save(ICON_DIR / f"stuckinthemd-{size}.png")

img.save(
    ICON_DIR / "stuckinthemd.ico",
    format="ICO",
    sizes=[(256, 256), (128, 128), (64, 64), (48, 48), (32, 32), (16, 16)],
)
print(f"Updated icons in {ICON_DIR}")
