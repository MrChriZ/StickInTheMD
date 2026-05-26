#pragma once

#include <string>

namespace stickinthemd {

class HtmlBuilder {
public:
  std::string preview_page(const std::string &body_html, bool dark = false) const;
  std::string print_page(const std::string &body_html,
                         const std::string &title = "Document") const;
};

} // namespace stickinthemd
