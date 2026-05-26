#pragma once

#include <string>

namespace stuckinthemd {

// Repairs files saved as a single JSON-escaped line (literal \n, wrapping quotes).
std::string normalize_document_text(std::string text);

} // namespace stuckinthemd
