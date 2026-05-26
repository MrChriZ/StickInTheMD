#include "stickinthemd/text_normalize.hpp"

#include "stickinthemd/json_util.hpp"

namespace stickinthemd {

namespace {

void replace_all(std::string &text, const std::string &from,
                 const std::string &to) {
  if (from.empty()) {
    return;
  }
  size_t pos = 0;
  while ((pos = text.find(from, pos)) != std::string::npos) {
    text.replace(pos, from.size(), to);
    pos += to.size();
  }
}

} // namespace

std::string normalize_document_text(std::string text) {
  if (text.size() >= 2 && text.front() == '"' && text.back() == '"') {
    text = parse_json_string(text);
  }

  const bool has_real_newlines = text.find('\n') != std::string::npos;
  const bool has_literal_newlines = text.find("\\n") != std::string::npos;
  if (!has_real_newlines && has_literal_newlines) {
    replace_all(text, "\\n", "\n");
    replace_all(text, "\\r", "\r");
    replace_all(text, "\\t", "\t");
  }

  return text;
}

} // namespace stickinthemd
