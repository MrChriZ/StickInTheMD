#include "stuckinthemd/document_store.hpp"

#include "stuckinthemd/text_normalize.hpp"

#include <fstream>
#include <sstream>

namespace stuckinthemd {

namespace {

std::string read_file(const std::filesystem::path &path, std::string &error) {
  std::ifstream in(path, std::ios::binary);
  if (!in) {
    error = "Could not open file for reading.";
    return {};
  }
  std::ostringstream buffer;
  buffer << in.rdbuf();
  return buffer.str();
}

bool write_file(const std::filesystem::path &path, const std::string &content,
                std::string &error) {
  std::ofstream out(path, std::ios::binary | std::ios::trunc);
  if (!out) {
    error = "Could not open file for writing.";
    return false;
  }
  out.write(content.data(), static_cast<std::streamsize>(content.size()));
  if (!out.good()) {
    error = "Failed while writing file.";
    return false;
  }
  return true;
}

} // namespace

LoadResult DocumentStore::load(const std::filesystem::path &path) const {
  LoadResult result;
  if (!std::filesystem::exists(path)) {
    result.error = "File does not exist.";
    return result;
  }
  result.content =
      normalize_document_text(read_file(path, result.error));
  if (!result.error.empty()) {
    return result;
  }
  result.ok = true;
  return result;
}

SaveResult DocumentStore::save(const std::filesystem::path &path,
                             const std::string &content) const {
  SaveResult result;
  std::error_code ec;
  const auto parent = path.parent_path();
  if (!parent.empty() && !std::filesystem::exists(parent)) {
    std::filesystem::create_directories(parent, ec);
    if (ec) {
      result.error = "Could not create parent directory.";
      return result;
    }
  }
  result.ok = write_file(path, content, result.error);
  return result;
}

} // namespace stuckinthemd
