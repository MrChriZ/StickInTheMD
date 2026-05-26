# Third-party dependencies

Most dependencies are **not** committed. Before the first build, run from the repository root:

```bash
bash scripts/bootstrap-third-party.sh
```

That script clones or downloads:

- [md4c](https://github.com/mity/md4c) (MIT)
- [webview](https://github.com/webview/webview) (MIT)
- [doctest](https://github.com/doctest/doctest) (MIT, tests only)
- [tinyfiledialogs](https://sourceforge.net/projects/tinyfiledialogs/) (zlib, vendored in `tinyfiledialogs/`)

See [THIRD_PARTY_NOTICES.md](../THIRD_PARTY_NOTICES.md) for license details.
