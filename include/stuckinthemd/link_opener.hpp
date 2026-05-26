#pragma once

#include <filesystem>
#include <optional>
#include <string>

namespace stuckinthemd {

enum class PreviewLinkAction { OpenInApp, OpenExternally };

struct PreviewLinkTarget {
  PreviewLinkAction action = PreviewLinkAction::OpenExternally;
  /** Absolute filesystem path (markdown) or URL / path string for ShellExecute / xdg-open. */
  std::string target;
};

/** True for http(s), mailto, tel, and protocol-relative // URLs. */
bool is_external_href(const std::string &href);

/** True if the path looks like a Markdown document we should open in the editor. */
bool is_markdown_file(const std::filesystem::path &path);

/**
 * Resolve a preview anchor href against the open document.
 * Returns nullopt for in-page anchors (#...) and unresolvable links.
 */
std::optional<PreviewLinkTarget>
resolve_preview_link(const std::filesystem::path &document_path,
                     const std::string &href);

/** Open a URL or file with the OS default handler (browser, mail client, etc.). */
bool open_with_system_default(const std::string &url_or_path);

} // namespace stuckinthemd
