#pragma once

#include <string>

namespace stickinthemd {

class MarkdownRenderer {
public:
  std::string to_html_body(const std::string &markdown) const;
};

} // namespace stickinthemd
