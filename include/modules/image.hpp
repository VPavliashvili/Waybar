#pragma once

#include <fmt/format.h>
#include <gtkmm/image.h>

#include <csignal>
#include <string>

#include "ALabel.hpp"
#include "gtkmm/box.h"
#include "util/command.hpp"
#include "util/json.hpp"
#include "util/sleeper_thread.hpp"

namespace waybar::modules {

namespace image {

class IStrategy {
 public:
  virtual ~IStrategy() = default;
  virtual void update() = 0;
};

class ImageStrategy : public IStrategy {
 public:
  ImageStrategy(const std::string&, const Json::Value&, const std::string&, Gtk::EventBox&, bool);
  ~ImageStrategy() override = default;
  void update() override;

 private:
  void parseOutputRaw();

  util::command::res output_;
  Json::Value config_;
  Gtk::Image image_;
  std::string path_;
  std::string tooltip_;
  int size_;
  Gtk::Box box_;
  bool hasTooltip_;
};

}  // namespace image

class Image : public AModule {
 public:
  Image(const std::string&, const Json::Value&);
  virtual ~Image() = default;
  auto update() -> void override;
  void refresh(int /*signal*/) override;

 private:
  void delayWorker();
  void handleEvent();
  // void parseOutputRaw();
  static std::unique_ptr<image::IStrategy> getStrategy(const std::string&, const Json::Value&,
                                                       const std::string&, Gtk::EventBox&, bool);

  int interval_;
  std::unique_ptr<image::IStrategy> strategy_;
  util::SleeperThread thread_;
};

}  // namespace waybar::modules
