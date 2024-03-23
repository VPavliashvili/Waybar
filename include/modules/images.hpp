#pragma once

#include <fmt/format.h>
#include <gtkmm/image.h>
#include <spdlog/spdlog.h>

#include <csignal>
#include <string>
#include <utility>

#include "ALabel.hpp"
#include "AModule.hpp"
#include "glibmm/fileutils.h"
#include "gtkmm/box.h"
#include "gtkmm/image.h"
#include "gtkmm/widget.h"
#include "util/command.hpp"
#include "util/json.hpp"
#include "util/sleeper_thread.hpp"

namespace waybar::modules {

struct ImageData {
  std::string path;
  std::string status;
  std::string tooltip;
};

class Images : public AModule {
 public:
  Images(const std::string &, const Json::Value &);
  virtual ~Images() = default;
  auto update() -> void override;
  void refresh(int /*signal*/) override;

 private:
  void delayWorker();
  void setImagesData(const Json::Value &);
  void draw();
  void resetBoxAndMemory();

  Json::Value config_;
  Gtk::Box box_;
  std::vector<ImageData> images_data_;
  std::vector<std::unique_ptr<Gtk::Image>> gtk_container_;
  int size_;
  int interval_;

  util::SleeperThread thread_;
};

}  // namespace waybar::modules
