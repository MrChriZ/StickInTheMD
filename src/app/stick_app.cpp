#include "stickinthemd/stick_app.hpp"

#include "stickinthemd/assets.hpp"
#include "stickinthemd/file_dialogs.hpp"
#include "stickinthemd/json_util.hpp"
#include "stickinthemd/color_theme.hpp"
#include "stickinthemd/version.hpp"
#include "stickinthemd/view_mode.hpp"

#include <webview/detail/json.hh>
#include <webview/webview.h>

#include <fstream>
#include <iostream>
#include <sstream>

namespace stickinthemd {

namespace {

std::string decode_bind_arg_at(const std::string &args, int index) {
  if (args.empty()) {
    return {};
  }
  if (args.front() == '[') {
    return webview::detail::json_parse(args, "", index);
  }
  return index == 0 ? args : std::string{};
}

std::string decode_bind_args(const std::string &args) {
  return decode_bind_arg_at(args, 0);
}

std::filesystem::path temp_dir() {
  auto dir = std::filesystem::temp_directory_path() / "stickinthemd";
  std::error_code ec;
  std::filesystem::create_directories(dir, ec);
  return dir;
}

} // namespace

StickApp::StickApp() = default;

StickApp::~StickApp() { stop_autosave_worker(); }

bool StickApp::write_temp_html(const std::filesystem::path &path,
                             const std::string &html) const {
  std::ofstream out(path, std::ios::binary | std::ios::trunc);
  if (!out) {
    return false;
  }
  out.write(html.data(), static_cast<std::streamsize>(html.size()));
  return out.good();
}

std::string StickApp::build_state_json() const {
  std::ostringstream json;
  json << "{";
  json << "\"path\":";
  if (controller_.document().has_path()) {
    json << json_escape(controller_.document().path()->string());
  } else {
    json << "null";
  }
  json << ",\"dirty\":" << (controller_.document().is_dirty() ? "true" : "false");
  json << ",\"autosaveEnabled\":"
       << (controller_.settings().autosave_enabled ? "true" : "false");
  json << ",\"autosaveIntervalMs\":"
       << controller_.settings().autosave_interval.count();
  json << ",\"viewMode\":"
       << json_escape(std::string(view_mode_name(controller_.settings().view_mode)));
  json << ",\"distractionFree\":"
       << (controller_.settings().distraction_free ? "true" : "false");
  json << "}";
  return json.str();
}

namespace {

std::string build_settings_json(const AppController &controller) {
  std::ostringstream json;
  json << "{";
  json << "\"autosaveEnabled\":"
       << (controller.settings().autosave_enabled ? "true" : "false");
  json << ",\"viewMode\":"
       << json_escape(std::string(view_mode_name(controller.settings().view_mode)));
  json << ",\"distractionFree\":"
       << (controller.settings().distraction_free ? "true" : "false");
  json << ",\"colorTheme\":"
       << json_escape(
              std::string(color_theme_name(controller.settings().color_theme)));
  json << ",\"version\":" << json_escape(app_version());
  json << "}";
  return json.str();
}

} // namespace

void StickApp::setup_bindings() {
  webview_->bind("renderMarkdown", [this](const std::string &req) -> std::string {
    const auto markdown = decode_bind_arg_at(req, 0);
    const auto appearance = decode_bind_arg_at(req, 1);
    const bool dark = appearance == "dark";
    return json_escape(controller_.preview_html_for(markdown, dark));
  });

  webview_->bind("getBootDocument", [this](const std::string &) -> std::string {
    if (!controller_.document().has_path() &&
        controller_.document().content().empty()) {
      return "null";
    }
    std::ostringstream json;
    json << "{";
    if (controller_.document().has_path()) {
      json << "\"path\":"
           << json_escape(controller_.document().path()->string());
    } else {
      json << "\"path\":null";
    }
    json << ",\"content\":"
         << json_escape(controller_.document().content()) << "}";
    return json.str();
  });

  webview_->bind("notifyContentChanged", [this](const std::string &req) {
    controller_.update_content(decode_bind_args(req));
    return std::string("null");
  });

  webview_->bind("getState", [this](const std::string &) -> std::string {
    return build_state_json();
  });

  webview_->bind("setAutosaveEnabled", [this](const std::string &req) -> std::string {
    controller_.settings().autosave_enabled = req.find("true") != std::string::npos;
    return "true";
  });

  webview_->bind("setViewMode", [this](const std::string &req) -> std::string {
    const auto name = decode_bind_args(req);
    controller_.settings().view_mode = view_mode_from_string(name);
    return json_escape(std::string(view_mode_name(controller_.settings().view_mode)));
  });

  webview_->bind("setDistractionFree", [this](const std::string &req) -> std::string {
    controller_.settings().distraction_free = req.find("true") != std::string::npos;
    return controller_.settings().distraction_free ? "true" : "false";
  });

  webview_->bind("setColorTheme", [this](const std::string &req) -> std::string {
    controller_.settings().color_theme =
        color_theme_from_string(decode_bind_args(req));
    return json_escape(
        std::string(color_theme_name(controller_.settings().color_theme)));
  });

  webview_->bind("newDocument", [this](const std::string &) -> std::string {
    controller_.new_document();
    return build_state_json();
  });

  webview_->bind("openDroppedFile", [this](const std::string &req) -> std::string {
    const auto path_hint = decode_bind_arg_at(req, 0);
    const auto content = decode_bind_arg_at(req, 1);
    const auto opened = controller_.open_dropped(
        path_hint.empty() ? std::filesystem::path{} : std::filesystem::path{path_hint},
        content);
    if (!opened.ok) {
      return std::string(R"({"ok":false,"error":)") + json_escape(opened.error) + "}";
    }
    std::ostringstream json;
    json << R"({"ok":true,"path":)";
    if (opened.path) {
      json << json_escape(opened.path->string());
    } else {
      json << "null";
    }
    json << R"(,"content":)" << json_escape(controller_.document().content()) << "}";
    return json.str();
  });

  webview_->bind("openDocument", [this](const std::string &) -> std::string {
    const auto path = pick_open_markdown();
    if (!path) {
      return R"({"ok":false,"error":"cancelled"})";
    }
    const auto opened = controller_.open_path(*path);
    if (!opened.ok) {
      return std::string(R"({"ok":false,"error":)") + json_escape(opened.error) + "}";
    }
    return std::string(R"({"ok":true,"path":)") + json_escape(path->string()) +
           R"(,"content":)" + json_escape(controller_.document().content()) + "}";
  });

  webview_->bind("openPath", [this](const std::string &req) -> std::string {
    const auto path_str = decode_bind_args(req);
    if (path_str.empty()) {
      return R"({"ok":false,"error":"No path given"})";
    }
    const auto opened = controller_.open_path(path_str);
    if (!opened.ok) {
      return std::string(R"({"ok":false,"error":)") + json_escape(opened.error) + "}";
    }
    const auto out_path =
        opened.path ? opened.path->string() : std::filesystem::path{path_str}.string();
    return std::string(R"({"ok":true,"path":)") + json_escape(out_path) +
           R"(,"content":)" + json_escape(controller_.document().content()) + "}";
  });

  webview_->bind("saveDocument", [this](const std::string &req) -> std::string {
    const bool save_as = req.find("true") != std::string::npos;
    SaveResultInfo saved;
    if (save_as || !controller_.document().has_path()) {
      const auto path = pick_save_markdown(controller_.document().has_path()
                                               ? *controller_.document().path()
                                               : std::filesystem::path{});
      if (!path) {
        return R"({"ok":false,"error":"cancelled"})";
      }
      saved = controller_.save_as_path(*path);
    } else {
      saved = controller_.save();
    }
    if (!saved.ok) {
      return std::string(R"({"ok":false,"error":)") + json_escape(saved.error) + "}";
    }
    return std::string(R"({"ok":true,"path":)") +
           json_escape(saved.path->string()) + "}";
  });

  webview_->bind("printDocument", [this](const std::string &) -> std::string {
    webview_->eval("window.printPreview(" +
                   json_escape(controller_.print_html()) + ");");
    return R"({"ok":true})";
  });

  webview_->bind("exportPdf", [this](const std::string &) -> std::string {
    std::filesystem::path suggested{"document.pdf"};
    if (controller_.document().has_path()) {
      suggested = *controller_.document().path();
      suggested.replace_extension(".pdf");
    }
    const auto pdf_path = pick_save_markdown(suggested);
    if (!pdf_path) {
      return R"({"ok":false,"error":"cancelled"})";
    }
    auto out = *pdf_path;
    if (out.extension() != ".pdf") {
      out.replace_extension(".pdf");
    }
    const auto html_path = temp_dir() / "export.html";
    if (!write_temp_html(html_path, controller_.print_html())) {
      return R"({"ok":false,"error":"Could not prepare export HTML."})";
    }
    const auto exported = pdf_exporter_.export_html_file(html_path, out);
    if (!exported.ok) {
      webview_->eval("window.printPreview(" +
                     json_escape(controller_.print_html()) + ");");
      return R"({"ok":false,"fallback":"print"})";
    }
    return std::string(R"({"ok":true,"path":)") + json_escape(out.string()) +
           R"(,"method":)" + json_escape(exported.method) + "}";
  });
}

void StickApp::start_autosave_worker() {
  autosave_stop_ = false;
  autosave_thread_ = std::thread([this] {
    while (!autosave_stop_.load()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(400));
      const auto now = std::chrono::steady_clock::now();
      if (!controller_.should_autosave(now)) {
        continue;
      }
      const auto saved = controller_.perform_autosave();
      if (saved.ok) {
        controller_.note_autosave(now);
        webview_->dispatch([this, path = saved.path->string()] {
          webview_->eval("window.applySaveResult(" + json_escape(path) + ");");
          webview_->eval("window.showMessage('Auto-saved');");
        });
      }
    }
  });
}

void StickApp::stop_autosave_worker() {
  autosave_stop_ = true;
  if (autosave_thread_.joinable()) {
    autosave_thread_.join();
  }
}

int StickApp::run(int argc, char *argv[]) {
  if (argc > 1) {
    const std::filesystem::path arg_path = argv[1];
    controller_.open_path(arg_path);
  }

  webview_ = std::make_unique<webview::webview>(true, nullptr); // webview = browser_engine
  webview_->set_title(std::string("StickInTheMD ") + app_version());
  webview_->set_size(1200, 800, WEBVIEW_HINT_NONE);

  const auto main_window = webview_->window();
  if (main_window.ok() && main_window.value() != nullptr) {
    set_file_dialog_parent(main_window.value());
  }

  setup_bindings();
  start_autosave_worker();

  webview_->set_html(load_ui_html());
  webview_->eval(
      "window.boot(" + build_settings_json(controller_) + ");");

  webview_->run();
  stop_autosave_worker();
  return 0;
}

} // namespace stickinthemd

int main(int argc, char *argv[]) {
  try {
    stickinthemd::StickApp app;
    return app.run(argc, argv);
  } catch (const std::exception &e) {
    std::cerr << "StickInTheMD failed: " << e.what() << '\n';
    return 1;
  }
}
