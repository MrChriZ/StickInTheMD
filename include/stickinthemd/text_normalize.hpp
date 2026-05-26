#pragma once

#include <string>

namespace stickinthemd {

// Repairs files saved as a single JSON-escaped line (literal \n, wrapping quotes).
std::string normalize_document_text(std::string text);

} // namespace stickinthemd
