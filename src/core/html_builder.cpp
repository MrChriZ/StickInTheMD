#include "stuckinthemd/html_builder.hpp"

#include <cctype>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <optional>
#include <system_error>
#include <vector>

namespace stuckinthemd {

namespace {

std::string percent_encode_path(const std::string &path) {
  std::string out;
  out.reserve(path.size() + 16);
  for (const unsigned char c : path) {
    if (std::isalnum(c) != 0 || c == '-' || c == '_' || c == '.' || c == '/' ||
        c == ':') {
      out.push_back(static_cast<char>(c));
    } else {
      char hex[4];
      std::snprintf(hex, sizeof(hex), "%%%02X", c);
      out += hex;
    }
  }
  return out;
}

std::string base_tag_for_dir(const std::filesystem::path &resource_dir) {
  if (resource_dir.empty()) {
    return {};
  }
  return "<base href=\"" + path_to_file_url(resource_dir) + "\">";
}

bool is_non_embeddable_src(const std::string &src) {
  if (src.empty()) {
    return true;
  }
  const auto lower = [](char c) { return static_cast<char>(std::tolower(static_cast<unsigned char>(c))); };
  auto starts = [&](const char *prefix) {
    const size_t n = std::strlen(prefix);
    if (src.size() < n) {
      return false;
    }
    for (size_t i = 0; i < n; ++i) {
      if (lower(src[i]) != lower(prefix[i])) {
        return false;
      }
    }
    return true;
  };
  return starts("http://") || starts("https://") || starts("data:") ||
         starts("file://") || starts("//");
}

std::string mime_for_image_path(const std::filesystem::path &path) {
  const auto ext = path.extension().string();
  std::string lower;
  lower.reserve(ext.size());
  for (char c : ext) {
    lower.push_back(
        static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
  }
  if (lower == ".jpg" || lower == ".jpeg") {
    return "image/jpeg";
  }
  if (lower == ".gif") {
    return "image/gif";
  }
  if (lower == ".webp") {
    return "image/webp";
  }
  if (lower == ".svg") {
    return "image/svg+xml";
  }
  if (lower == ".bmp") {
    return "image/bmp";
  }
  return "image/png";
}

std::string base64_encode(const std::vector<unsigned char> &bytes) {
  static constexpr char k_table[] =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  std::string out;
  out.reserve(((bytes.size() + 2) / 3) * 4);
  size_t i = 0;
  while (i + 2 < bytes.size()) {
    const unsigned n =
        (static_cast<unsigned>(bytes[i]) << 16) |
        (static_cast<unsigned>(bytes[i + 1]) << 8) |
        static_cast<unsigned>(bytes[i + 2]);
    out.push_back(k_table[(n >> 18) & 63]);
    out.push_back(k_table[(n >> 12) & 63]);
    out.push_back(k_table[(n >> 6) & 63]);
    out.push_back(k_table[n & 63]);
    i += 3;
  }
  if (i < bytes.size()) {
    unsigned n = static_cast<unsigned>(bytes[i]) << 16;
    if (i + 1 < bytes.size()) {
      n |= static_cast<unsigned>(bytes[i + 1]) << 8;
    }
    out.push_back(k_table[(n >> 18) & 63]);
    out.push_back(k_table[(n >> 12) & 63]);
    out.push_back(i + 1 < bytes.size() ? k_table[(n >> 6) & 63] : '=');
    out.push_back('=');
  }
  return out;
}

std::optional<std::string>
read_image_data_uri(const std::filesystem::path &path) {
  constexpr std::size_t k_max_bytes = 10 * 1024 * 1024;
  std::error_code ec;
  if (!std::filesystem::is_regular_file(path, ec)) {
    return std::nullopt;
  }
  const auto size = std::filesystem::file_size(path, ec);
  if (ec || size == 0 || size > k_max_bytes) {
    return std::nullopt;
  }
  std::ifstream in(path, std::ios::binary);
  if (!in) {
    return std::nullopt;
  }
  std::vector<unsigned char> bytes(static_cast<std::size_t>(size));
  in.read(reinterpret_cast<char *>(bytes.data()),
          static_cast<std::streamsize>(bytes.size()));
  if (!in.good()) {
    return std::nullopt;
  }
  return "data:" + mime_for_image_path(path) + ";base64," + base64_encode(bytes);
}

std::filesystem::path resolve_image_path(const std::string &src,
                                         const std::filesystem::path &resource_dir) {
  std::filesystem::path relative(src);
  if (relative.is_absolute()) {
    return relative;
  }
  const bool windows_drive = src.size() >= 2 && std::isalpha(static_cast<unsigned char>(src[0])) != 0 &&
                             src[1] == ':';
  if (windows_drive) {
    return relative;
  }
  return resource_dir / relative;
}

std::string embed_local_images_for_preview_impl(
    const std::string &body_html, const std::filesystem::path &resource_dir) {
  if (resource_dir.empty()) {
    return body_html;
  }
  std::string html = body_html;
  size_t pos = 0;
  while (true) {
    const size_t img = html.find("<img", pos);
    if (img == std::string::npos) {
      break;
    }
    const size_t src_attr = html.find("src=\"", img);
    if (src_attr == std::string::npos || src_attr > img + 80) {
      pos = img + 4;
      continue;
    }
    const size_t val_start = src_attr + 5;
    const size_t val_end = html.find('"', val_start);
    if (val_end == std::string::npos) {
      pos = img + 4;
      continue;
    }
    const std::string src = html.substr(val_start, val_end - val_start);
    if (is_non_embeddable_src(src)) {
      pos = val_end;
      continue;
    }
    const auto file_path = resolve_image_path(src, resource_dir);
    const auto data_uri = read_image_data_uri(file_path);
    if (!data_uri) {
      pos = val_end;
      continue;
    }
    html.replace(val_start, val_end - val_start, *data_uri);
    pos = val_start + data_uri->size();
  }
  return html;
}

} // namespace

std::string embed_local_images_for_preview(
    const std::string &body_html, const std::filesystem::path &resource_dir) {
  return embed_local_images_for_preview_impl(body_html, resource_dir);
}

std::string path_to_file_url(const std::filesystem::path &path) {
  std::error_code ec;
  auto abs = std::filesystem::absolute(path, ec);
  if (ec) {
    abs = path;
  }
  const std::string encoded = percent_encode_path(abs.lexically_normal().generic_string());
  std::string url;
  if (encoded.size() >= 2 && std::isalpha(static_cast<unsigned char>(encoded[0])) != 0 &&
      encoded[1] == ':') {
    url = "file:///" + encoded;
  } else if (!encoded.empty() && encoded.front() == '/') {
    url = "file://" + encoded;
  } else {
    url = "file:///" + encoded;
  }
  if (!ec && std::filesystem::is_directory(abs)) {
    if (url.back() != '/') {
      url.push_back('/');
    }
  }
  return url;
}

namespace {

constexpr const char *k_preview_css = R"css(
:root {
  color-scheme: light;
  --bg: #fffcfa;
  --fg: #1c1917;
  --muted: #57534e;
  --code-bg: #ebe8e4;
  --border: rgba(28, 25, 23, 0.14);
  --accent: #6366f1;
}
html[data-theme="dark"] {
  color-scheme: dark;
  --bg: #141110;
  --fg: #fafaf9;
  --muted: #a8a29e;
  --code-bg: #292524;
  --border: rgba(255, 255, 255, 0.12);
  --accent: #818cf8;
}
* { box-sizing: border-box; }
body {
  margin: 0;
  padding: 2rem 2.5rem;
  font-family: "Segoe UI", system-ui, -apple-system, sans-serif;
  font-size: 16px;
  line-height: 1.65;
  color: var(--fg);
  background: var(--bg);
}
h1, h2, h3, h4, h5, h6 { line-height: 1.25; margin-top: 1.5em; margin-bottom: 0.5em; }
h1 { font-size: 2em; border-bottom: 1px solid var(--border); padding-bottom: 0.3em; }
h2 { font-size: 1.5em; border-bottom: 1px solid var(--border); padding-bottom: 0.25em; }
a { color: var(--accent); }
code, pre { font-family: "Cascadia Code", "Fira Code", Consolas, monospace; font-size: 0.9em; }
code { background: var(--code-bg); padding: 0.15em 0.35em; border-radius: 4px; }
pre { background: var(--code-bg); padding: 1em; overflow-x: auto; border-radius: 6px; }
pre code { background: none; padding: 0; }
blockquote {
  margin: 1em 0;
  padding: 0.25em 1em;
  border-left: 4px solid var(--accent);
  color: var(--muted);
}
table { border-collapse: collapse; width: 100%; margin: 1em 0; }
th, td { border: 1px solid var(--border); padding: 0.5em 0.75em; }
th { background: var(--code-bg); }
hr { border: none; border-top: 1px solid var(--border); margin: 2em 0; }
img { max-width: 100%; }
ul.task-list { list-style: none; padding-left: 1.2em; }
input[type="checkbox"] { margin-right: 0.4em; }
)css";

constexpr const char *k_print_css = R"css(
@page { margin: 2cm; }
body { font-family: Georgia, "Times New Roman", serif; font-size: 11pt; line-height: 1.5; color: #000; background: #fff; }
pre, code { font-family: Consolas, monospace; font-size: 9pt; }
)css";

} // namespace

std::string HtmlBuilder::preview_page(const std::string &body_html, const bool dark,
                                      const std::filesystem::path &resource_dir) const {
  const char *theme_attr = dark ? " data-theme=\"dark\"" : "";
  std::string body = body_html;
  if (!resource_dir.empty()) {
    body = embed_local_images_for_preview(body_html, resource_dir);
  }
  return std::string("<!DOCTYPE html><html") + theme_attr +
         "><head><meta charset=\"utf-8\">" + base_tag_for_dir(resource_dir) +
         "<style>" + k_preview_css + "</style></head><body>" + body +
         "</body></html>";
}

std::string HtmlBuilder::print_page(const std::string &body_html,
                                    const std::string &title,
                                    const std::filesystem::path &resource_dir) const {
  return std::string("<!DOCTYPE html><html><head><meta charset=\"utf-8\"><title>") +
         title + "</title>" + base_tag_for_dir(resource_dir) + "<style>" + k_preview_css +
         k_print_css + "</style></head><body>" + body_html + "</body></html>";
}

} // namespace stuckinthemd
