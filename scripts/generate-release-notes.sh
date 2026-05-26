#!/usr/bin/env bash
# Extract release notes for a version from RELEASE_NOTES.md (POSIX; used on CI/Linux).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
VERSION="${1:-}"
OUTPUT="${2:-}"

if [[ -z "$VERSION" ]]; then
  VERSION="$(grep -E 'project\s*\(\s*StuckInTheMD\s+VERSION\s+[0-9.]+' "$ROOT/CMakeLists.txt" \
    | head -1 | sed -E 's/.*VERSION\s+([0-9.]+).*/\1/')"
fi

SOURCE="$ROOT/RELEASE_NOTES.md"
if [[ ! -f "$SOURCE" ]]; then
  echo "Missing RELEASE_NOTES.md" >&2
  exit 1
fi

mkdir -p "$ROOT/dist"
if [[ -z "$OUTPUT" ]]; then
  OUTPUT="$ROOT/dist/release-notes-v${VERSION}.md"
fi

HEADER="## v${VERSION}"
if ! grep -qF "$HEADER" "$SOURCE"; then
  echo "No release notes section '$HEADER' in RELEASE_NOTES.md" >&2
  exit 1
fi

BODY="$(awk -v ver="$VERSION" '
  $0 == "## v" ver { found=1; next }
  found && /^## v[0-9]+\.[0-9]+\.[0-9]+[[:space:]]*$/ { exit }
  found { print }
' "$SOURCE")"

# Trim leading/trailing blank lines
BODY="$(printf '%s\n' "$BODY" | awk 'NF{p=1} p' | sed -e :a -e '/^\n*$/{$d;N;ba' -e '}' 2>/dev/null || printf '%s' "$BODY" | sed '/./,$!d')"

if [[ -z "${BODY//[[:space:]]/}" ]]; then
  echo "Release notes section '$HEADER' is empty" >&2
  exit 1
fi

{
  printf '# StuckInTheMD v%s\n\n' "$VERSION"
  printf '%s\n' "$BODY"
  printf '\n## Install\n\n'
  printf 'Download **StuckInTheMD-%s-setup.exe** from the assets below and run the installer.\n' "$VERSION"
} >"$OUTPUT"

echo "Wrote release notes: $OUTPUT"
