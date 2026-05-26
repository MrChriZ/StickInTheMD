#pragma once

#include <filesystem>
#include <string>

namespace stuckinthemd {

/** file:/// URL for a directory or file (local preview / print). */
std::string path_to_file_url(const std::filesystem::path &path);

class HtmlBuilder {
public:
  std::string preview_page(const std::string &body_html, bool dark = false,
                           const std::filesystem::path &resource_dir = {}) const;
  std::string print_page(const std::string &body_html,
                         const std::string &title = "Document",
                         const std::filesystem::path &resource_dir = {}) const;
};

} // namespace stuckinthemd
