#pragma once

#include <cstdio>
#include <sstream>
#include <string>

namespace stuckinthemd {

inline std::string json_escape(const std::string &text) {
  std::ostringstream out;
  out << '"';
  for (unsigned char c : text) {
    switch (c) {
    case '"':
      out << "\\\"";
      break;
    case '\\':
      out << "\\\\";
      break;
    case '\b':
      out << "\\b";
      break;
    case '\f':
      out << "\\f";
      break;
    case '\n':
      out << "\\n";
      break;
    case '\r':
      out << "\\r";
      break;
    case '\t':
      out << "\\t";
      break;
    default:
      if (c < 0x20) {
        char buf[8];
        snprintf(buf, sizeof(buf), "\\u%04x", static_cast<unsigned>(c));
        out << buf;
      } else {
        out << static_cast<char>(c);
      }
    }
  }
  out << '"';
  return out.str();
}

inline std::string parse_json_string(const std::string &req) {
  auto start = req.find('"');
  if (start == std::string::npos) {
    return req;
  }
  std::string out;
  bool escape = false;
  for (size_t i = start + 1; i < req.size(); ++i) {
    const char c = req[i];
    if (escape) {
      switch (c) {
      case 'n':
        out.push_back('\n');
        break;
      case 'r':
        out.push_back('\r');
        break;
      case 't':
        out.push_back('\t');
        break;
      case '\\':
      case '"':
        out.push_back(c);
        break;
      default:
        out.push_back(c);
        break;
      }
      escape = false;
      continue;
    }
    if (c == '\\') {
      escape = true;
      continue;
    }
    if (c == '"') {
      break;
    }
    out.push_back(c);
  }
  return out;
}

// Webview passes the bound params field: a JSON array string like ["# Hi\n"].
// When the payload is already decoded (no leading '['), use it as-is so
// quotes inside Markdown are not mistaken for JSON delimiters.
inline std::string bind_first_arg(const std::string &args) {
  if (args.empty()) {
    return {};
  }
  if (args.front() != '[') {
    return args;
  }
  const auto quote = args.find('"');
  if (quote == std::string::npos) {
    return {};
  }
  return parse_json_string(args.substr(quote));
}

inline std::string markdown_from_bind_request(const std::string &req) {
  return bind_first_arg(req);
}

} // namespace stuckinthemd
