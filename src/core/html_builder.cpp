#include "stuckinthemd/html_builder.hpp"

#include <cctype>
#include <cstdio>
#include <system_error>

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

} // namespace

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
  return std::string("<!DOCTYPE html><html") + theme_attr +
         "><head><meta charset=\"utf-8\">" + base_tag_for_dir(resource_dir) +
         "<style>" + k_preview_css + "</style></head><body>" + body_html +
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
