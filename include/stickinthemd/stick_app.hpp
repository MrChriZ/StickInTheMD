#pragma once

#include "stickinthemd/app_controller.hpp"
#include "stickinthemd/pdf_exporter.hpp"

#include <atomic>
#include <chrono>
#include <filesystem>
#include <memory>
#include <thread>

#include <webview/webview.h>

namespace stickinthemd {

class StickApp {
public:
  StickApp();
  ~StickApp();

  int run(int argc, char *argv[]);

private:
  void setup_bindings();
  void start_autosave_worker();
  void stop_autosave_worker();
  std::string build_state_json() const;
  bool write_temp_html(const std::filesystem::path &path,
                       const std::string &html) const;

  AppController controller_;
  PdfExporter pdf_exporter_;
  std::unique_ptr<webview::webview> webview_;
  std::thread autosave_thread_;
  std::atomic<bool> autosave_stop_{false};
};

} // namespace stickinthemd
