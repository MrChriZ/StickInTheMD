#pragma once

#include <string>

namespace stuckinthemd {

class MarkdownRenderer {
public:
  std::string to_html_body(const std::string &markdown) const;
};

} // namespace stuckinthemd
