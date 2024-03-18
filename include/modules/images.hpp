#pragma once

#include <fmt/format.h>
#include <gtkmm/image.h>
#include <spdlog/spdlog.h>

#include <csignal>
#include <string>

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

class Images : public AModule {
 public:
  Images(const std::string &, const Json::Value &);
  virtual ~Images() = default;
  auto update() -> void override;
  void refresh(int /*signal*/) override;

 private:
  void delayWorker();
  void setEntries(const Json::Value&);

  Json::Value config_;
  Gtk::Box box_;
  std::vector<std::string> entries_;
  int size_;
  int interval_;

  util::SleeperThread thread_;
};

}  // namespace waybar::modules
