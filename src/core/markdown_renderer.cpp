#include "stickinthemd/markdown_renderer.hpp"

extern "C" {
#include "md4c-html.h"
}

#include <string>

namespace stickinthemd {

namespace {

void append_html(const MD_CHAR *data, MD_SIZE size, void *userdata) {
  auto *out = static_cast<std::string *>(userdata);
  out->append(data, size);
}

} // namespace

std::string MarkdownRenderer::to_html_body(const std::string &markdown) const {
  std::string html;
  const unsigned parser_flags =
      MD_FLAG_TABLES | MD_FLAG_STRIKETHROUGH | MD_FLAG_TASKLISTS |
      MD_FLAG_PERMISSIVEAUTOLINKS | MD_FLAG_FOOTNOTES | MD_FLAG_WIKILINKS;
  const unsigned renderer_flags = MD_HTML_FLAG_SKIP_UTF8_BOM;

  const int rc = md_html(markdown.data(), static_cast<MD_SIZE>(markdown.size()),
                         append_html, &html, parser_flags, renderer_flags);
  if (rc != 0) {
    return "<p><em>Failed to render markdown.</em></p>";
  }
  return html;
}

} // namespace stickinthemd
