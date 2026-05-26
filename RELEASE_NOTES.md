# StuckInTheMD release notes

Edit the section for the version you are shipping **before** bumping `CMakeLists.txt` and tagging.
The installer build writes `dist/release-notes-vX.Y.Z.md` from the matching `## vX.Y.Z` block below.

## v1.0.9

- **Preview images**: local images in Markdown (e.g. `assets/screenshot.png`) are embedded in the preview so they display reliably in WebView2.
- **Smart links**: links in preview open in the default browser or app; `.md` links open in StuckInTheMD.
- **Preview bind fix** (from 1.0.8): `renderMarkdown` returns JSON-encoded HTML for the webview.

## v1.0.8

- Fix broken preview (“Preview failed”) by returning JSON-encoded HTML from the `renderMarkdown` webview bind.

## v1.0.7

- Fix empty preview by rendering Markdown HTML in the preview iframe via `srcdoc` instead of `file://` URLs (blocked in WebView2 subframes).

## v1.0.6

- Linux CI fix: rename `ExternalFileChange::Kind::None` to `NoChange` (conflict with X11 `None` macro).
- Release workflow runs the full test matrix before publishing the Windows installer.

## v1.0.5

- Fix preview regression introduced with external file watching.

## v1.0.4

- Reload when the open file changes on disk (auto when clean, prompt when dirty).

## v1.0.3

- Maintenance release (version bump).

## v1.0.2

- Installer publisher metadata (Leftoverbits).

## v1.0.1

- Rename project from StickInTheMD to StuckInTheMD.

## v1.0.0

- Initial public release: Markdown editor with live preview, save/open, print, distraction-free mode, Windows installer.
