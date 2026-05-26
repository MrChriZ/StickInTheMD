# Third-party notices

StuckInTheMD bundles or links the following third-party software. This file
satisfies attribution requirements for binary distributions (installers,
releases).

## StuckInTheMD

Copyright (c) StuckInTheMD contributors

Licensed under the MIT License. See [LICENSE](LICENSE).

## md4c

Copyright (c) 2016-2026 Martin Mitáš

<https://github.com/mity/md4c>

Licensed under the MIT License. Statically linked into the application.

## webview

Copyright (c) 2017 Serge Zaitsev  
Copyright (c) 2022 Steffen André Langnes

<https://github.com/webview/webview>

Licensed under the MIT License. Statically linked into the application.

## tinyfiledialogs

Copyright (c) 2014-2017 Guillaume Vareille  
<http://tinyfiledialogs.sourceforge.net>

Licensed under the zlib License. Source is vendored in `third_party/tinyfiledialogs/`.

## doctest (tests only)

Copyright (c) 2016-2023 Viktor Kirilov

<https://github.com/doctest/doctest>

Licensed under the MIT License. Used only when building and running unit tests;
not included in release binaries.

## Microsoft Edge WebView2

On Windows, the application uses the **Microsoft Edge WebView2** runtime provided
by the operating system (or installed separately on Windows 10). The WebView2
SDK is fetched at build time via CMake for headers and linking; it is not
redistributed inside the installer.

- Runtime: <https://developer.microsoft.com/microsoft-edge/webview2/>
- SDK: subject to the [Microsoft WebView2 SDK license](https://www.nuget.org/packages/Microsoft.Web.WebView2)
