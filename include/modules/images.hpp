#pragma once

#include <fmt/format.h>
#include <gtkmm/image.h>
#include <spdlog/spdlog.h>

#include <string>

#include "AModule.hpp"
#include "gtkmm/box.h"
#include "gtkmm/image.h"
#include "util/sleeper_thread.hpp"

namespace waybar::modules {

class Images : public AModule {
 public:
  Images(const std::string&, const Json::Value&);
  virtual ~Images() = default;
  auto update() -> void override;

 private:
  void delayWorker();

  Json::Value config_;
  Gtk::Box box_;
  std::vector<std::string> entries_;
  int size_;
  int interval_;

  Gtk::Image img_;
  util::SleeperThread thread_;
};

}  // namespace waybar::modules
