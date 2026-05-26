#include "stuckinthemd/link_opener.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <system_error>

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <shellapi.h>
#endif

namespace stuckinthemd {

namespace {

std::string to_lower(std::string value) {
  for (char &c : value) {
    c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
  }
  return value;
}

bool starts_with_ignore_case(const std::string &text, const char *prefix) {
  const size_t n = std::strlen(prefix);
  if (text.size() < n) {
    return false;
  }
  for (size_t i = 0; i < n; ++i) {
    if (std::tolower(static_cast<unsigned char>(text[i])) !=
        std::tolower(static_cast<unsigned char>(prefix[i]))) {
      return false;
    }
  }
  return true;
}

std::string shell_single_quote(const std::string &text) {
  std::string quoted = "'";
  for (char c : text) {
    if (c == '\'') {
      quoted += "'\\''";
    } else {
      quoted += c;
    }
  }
  quoted += '\'';
  return quoted;
}

#if defined(_WIN32)
std::wstring utf8_to_wide(const std::string &text) {
  if (text.empty()) {
    return {};
  }
  const int size =
      MultiByteToWideChar(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()),
                          nullptr, 0);
  if (size <= 0) {
    return {};
  }
  std::wstring wide(static_cast<size_t>(size), L'\0');
  MultiByteToWideChar(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()),
                      wide.data(), size);
  return wide;
}
#endif

} // namespace

bool is_external_href(const std::string &href) {
  if (href.empty()) {
    return false;
  }
  if (starts_with_ignore_case(href, "http://") ||
      starts_with_ignore_case(href, "https://") ||
      starts_with_ignore_case(href, "mailto:") ||
      starts_with_ignore_case(href, "tel:") || starts_with_ignore_case(href, "//")) {
    return true;
  }
  return false;
}

bool is_markdown_file(const std::filesystem::path &path) {
  const auto ext = to_lower(path.extension().string());
  return ext == ".md" || ext == ".markdown" || ext == ".mdown" || ext == ".mkd" ||
         ext == ".mkdn";
}

bool open_with_system_default(const std::string &url_or_path) {
  if (url_or_path.empty()) {
    return false;
  }

#if defined(_WIN32)
  const auto wide = utf8_to_wide(url_or_path);
  if (wide.empty()) {
    return false;
  }
  const auto result = reinterpret_cast<intptr_t>(ShellExecuteW(
      nullptr, L"open", wide.c_str(), nullptr, nullptr, SW_SHOWNORMAL));
  return result > 32;
#elif defined(__APPLE__)
  const std::string cmd = "open " + shell_single_quote(url_or_path) + " >/dev/null 2>&1";
  return std::system(cmd.c_str()) == 0;
#else
  const std::string cmd =
      "xdg-open " + shell_single_quote(url_or_path) + " >/dev/null 2>&1";
  return std::system(cmd.c_str()) == 0;
#endif
}

std::optional<PreviewLinkTarget>
resolve_preview_link(const std::filesystem::path &document_path,
                     const std::string &href) {
  if (href.empty() || href.front() == '#') {
    return std::nullopt;
  }

  if (is_external_href(href)) {
    std::string url = href;
    if (starts_with_ignore_case(href, "//")) {
      url = "https:" + href;
    }
    return PreviewLinkTarget{PreviewLinkAction::OpenExternally, url};
  }

  std::filesystem::path target(href);
  const bool windows_drive =
      href.size() >= 2 && std::isalpha(static_cast<unsigned char>(href[0])) != 0 &&
      href[1] == ':';
  if (!target.is_absolute() && !windows_drive) {
    std::filesystem::path base;
    if (!document_path.empty()) {
      base = document_path.parent_path();
    }
    if (base.empty()) {
      base = std::filesystem::current_path();
    }
    target = base / target;
  }

  std::error_code ec;
  const auto normalized = std::filesystem::weakly_canonical(target, ec);
  if (!ec) {
    target = normalized;
  } else {
    target = target.lexically_normal();
  }

  if (is_markdown_file(target)) {
    return PreviewLinkTarget{PreviewLinkAction::OpenInApp, target.string()};
  }

  return PreviewLinkTarget{PreviewLinkAction::OpenExternally, target.string()};
}

} // namespace stuckinthemd
