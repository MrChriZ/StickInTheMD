# Contributing to StuckInTheMD

Thank you for your interest in contributing.

## Development setup

1. Clone the repository.
2. Fetch dependencies:

   ```bash
   bash scripts/bootstrap-third-party.sh
   ```

3. Configure and build:

   ```bash
   cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DSTUCKINTHEMD_BUILD_TESTS=ON
   cmake --build build
   ```

4. Run tests:

   ```bash
   ctest --test-dir build --output-on-failure
   ```

On Windows with Visual Studio, use `-G "Visual Studio 17 2022" -A x64` and
`ctest --test-dir build -C Release --output-on-failure`.

Shortcut scripts (same steps as CI):

```bash
bash scripts/run-tests.sh
```

```powershell
.\scripts\run-tests.ps1
```

See [README.md](README.md) for platform-specific dependencies (GTK/WebKit on
Linux, WebView2 on Windows 10, etc.).

## Releases

1. Run tests locally (`scripts/run-tests.sh` or `scripts/run-tests.ps1`).
2. Push to `main` and wait for the **CI** workflow to pass on GitHub.
3. Bump `project(StuckInTheMD VERSION …)` in `CMakeLists.txt`, commit, push.
4. Tag `vX.Y.Z` and push the tag.

The **Release** workflow runs the full test matrix on Linux, macOS, and Windows
before it builds or uploads the Windows installer. A failing test blocks the
release.

## Pull requests

- Keep changes focused; match existing code style (C++20, minimal comments).
- Ensure CI passes (Linux, macOS, Windows).
- Do not commit build output (`build/`, `dist/`) or bootstrapped clones under
  `third_party/md4c`, `third_party/webview`, or `third_party/doctest`.
- Update [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) if you add or change
  bundled third-party code.

## Reporting issues

Use the bug report template and include your OS, compiler, and steps to
reproduce.
