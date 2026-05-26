#pragma once

#include <filesystem>
#include <string>

namespace stickinthemd {

struct LoadResult {
  bool ok = false;
  std::string content;
  std::string error;
};

struct SaveResult {
  bool ok = false;
  std::string error;
};

class DocumentStore {
public:
  LoadResult load(const std::filesystem::path &path) const;
  SaveResult save(const std::filesystem::path &path,
                  const std::string &content) const;
};

} // namespace stickinthemd
