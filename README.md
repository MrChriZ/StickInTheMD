# StickInTheMD

![CI](https://github.com/MrChriZ/StickInTheMD/actions/workflows/ci.yml/badge.svg)

A lightweight, cross-platform Markdown editor with live preview.

Built with **modern C++20**, a small native core, and your operating system's built-in webview (no bundled Chromium/Electron).

## Features

- **Live preview** — edit Markdown on the left, formatted HTML on the right
- **Drag and drop** — drop `.md` or text files onto the window to open
- **Save / Save As** — native file dialogs
- **Auto-save** — optional, every 2 seconds when the file has a path
- **Print** — system print dialog via a print-optimized page
- **Export PDF** — saves via Edge/Chrome or `wkhtmltopdf` when installed; otherwise opens the print dialog (choose **Save as PDF** there)
- **Focus mode** — cycle Split / Editor-only / Preview-only; F11 hides the toolbar
- **Cross-platform** — Windows, macOS, Linux
- **CI** — GitHub Actions builds and tests on Linux, macOS, and Windows

## Dependencies

Build dependencies (md4c, webview, doctest) are fetched by
[`scripts/bootstrap-third-party.sh`](scripts/bootstrap-third-party.sh), which CI
runs automatically. `tinyfiledialogs` is vendored under
[`third_party/tinyfiledialogs/`](third_party/tinyfiledialogs/). See
[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) for licenses.

At **runtime**, the app uses what your OS already provides:

| Platform | Runtime |
|----------|---------|
| Windows 11 | WebView2 (built-in) |
| Windows 10 | [WebView2 runtime](https://developer.microsoft.com/microsoft-edge/webview2/) |
| macOS | WebKit (built-in) |
| Linux | GTK 3/4 + WebKitGTK (common on desktop distros) |

Optional: install Edge, Chrome, or `wkhtmltopdf` for one-click PDF export without the print dialog.

## Application icon

Icons live in `assets/icon/`. Windows uses `stickinthemd.ico` automatically. On macOS, run `bash scripts/build-macos-icon.sh` once (or let CI build it) to produce `stickinthemd.icns` for the `.app` bundle.

Open/Save file dialogs are parented to the main window so they stay modal, use the app icon, and do not get a separate taskbar button.

**Theme:** Toolbar button cycles System → Dark → Light (shortcut **Ctrl+Shift+D**). Choice is remembered in local storage; preview matches the active appearance.

## Bootstrap (required after clone)

Before your first build, run:

```bash
bash scripts/bootstrap-third-party.sh
```

**Version** is set in `CMakeLists.txt` (`project(StickInTheMD VERSION …)`). It appears in the window title, toolbar badge, and Windows installer filename.

## Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

### Windows (Visual Studio)

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
.\build\Release\stickinthemd.exe
.\build\Release\stickinthemd.exe notes.md
```

### Windows installer

Creates a single `dist\StickInTheMD-<version>-setup.exe` (~few MB) that embeds one `stickinthemd.exe` (static MSVC runtime, no extra DLLs). Preview uses the OS WebView2 engine (preinstalled on Windows 11; see below for Windows 10).

1. Install [Inno Setup 6](https://jrsoftware.org/isinfo.php).
2. Build the app, then compile the installer:

```powershell
.\scripts\build-windows-installer.ps1
```

During setup, **“Make StickInTheMD the default app for Markdown files”** is enabled by default. That registers `.md` and `.markdown` to open with StickInTheMD. On some Windows versions you may still confirm under **Settings → Apps → Default apps**.

Or, after a CMake build with Inno Setup on `PATH`:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DSTICKINTHEMD_BUILD_WINDOWS_INSTALLER=ON
cmake --build build --config Release --target stickinthemd_installer
```

**Windows 10:** install the [WebView2 runtime](https://developer.microsoft.com/microsoft-edge/webview2/) if the app window does not appear (one-time, shared by other apps).

### Linux (Debian/Ubuntu)

```bash
sudo apt install build-essential cmake libgtk-3-dev libwebkit2gtk-4.1-dev
cmake -S . -B build && cmake --build build
./build/stickinthemd
```

### macOS

```bash
cmake -S . -B build && cmake --build build
./build/stickinthemd
```

## Tests

```bash
cmake -S . -B build -DSTICKINTHEMD_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Keyboard shortcuts

| Shortcut | Action |
|----------|--------|
| Ctrl+O | Open |
| *(toolbar)* | **Recent** dropdown — last 10 opened file paths |
| Ctrl+S | Save |
| Ctrl+Shift+S | Save As |
| Ctrl+P | Print |
| Ctrl+\\ | Cycle view: Split → Editor → Preview |
| F11 | Toggle distraction-free (hide toolbar) |

## License

Application code: MIT. Third-party libraries are listed in
[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md). Windows installs also place
`THIRD_PARTY_NOTICES.txt` in the application directory.
