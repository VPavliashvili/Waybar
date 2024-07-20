#include "modules/image.hpp"

waybar::modules::Image::Image(const std::string& id, const Json::Value& config)
    : AModule(config, "image", id) {
  strategy_ = getStrategy(id, config, MODULE_CLASS, event_box_);

  interval_ = config_["interval"].asInt();

  if (interval_ == 0) {
    interval_ = INT_MAX;
  }

  delayWorker();
}

auto waybar::modules::Image::getStrategy(const std::string& id, const Json::Value& cfg,
                                         const std::string& module, Gtk::EventBox& evbox)
    -> std::unique_ptr<waybar::modules::image::IStrategy> {
  std::unique_ptr<waybar::modules::image::IStrategy> strat =
      std::make_unique<waybar::modules::image::ImageStrategy>(id, cfg, module, evbox);

  return strat;
}

void waybar::modules::Image::delayWorker() {
  thread_ = [this] {
    dp.emit();
    auto interval = std::chrono::seconds(interval_);
    thread_.sleep_for(interval);
  };
}

void waybar::modules::Image::refresh(int sig) {
  if (sig == SIGRTMIN + config_["signal"].asInt()) {
    thread_.wake_up();
  }
}

auto waybar::modules::Image::update() -> void {
  strategy_->update();

  // Glib::RefPtr<Gdk::Pixbuf> pixbuf;
  // if (config_["path"].isString()) {
  //   path_ = config_["path"].asString();
  // } else if (config_["exec"].isString()) {
  //   output_ = util::command::exec(config_["exec"].asString(), "");
  //   parseOutputRaw();
  // } else {
  //   path_ = "";
  // }
  // if (Glib::file_test(path_, Glib::FILE_TEST_EXISTS))
  //   pixbuf = Gdk::Pixbuf::create_from_file(path_, size_, size_);
  // else
  //   pixbuf = {};
  //
  // if (pixbuf) {
  //   if (tooltipEnabled() && !tooltip_.empty()) {
  //     if (box_.get_tooltip_markup() != tooltip_) {
  //       box_.set_tooltip_markup(tooltip_);
  //     }
  //   }
  //   image_.set(pixbuf);
  //   image_.show();
  //   box_.get_style_context()->remove_class("empty");
  // } else {
  //   image_.clear();
  //   image_.hide();
  //   box_.get_style_context()->add_class("empty");
  // }

  AModule::update();
}

// void waybar::modules::Image::parseOutputRaw() {
// std::istringstream output(output_.out);
// std::string line;
// int i = 0;
// while (getline(output, line)) {
//   if (i == 0) {
//     path_ = line;
//   } else if (i == 1) {
//     tooltip_ = line;
//   } else {
//     break;
//   }
//   i++;
// }
// }

waybar::modules::image::ImageStrategy::ImageStrategy(const std::string& id,
                                                     const Json::Value& config,
                                                     const std::string& module,
                                                     Gtk::EventBox& evbox)
    : waybar::modules::image::IStrategy(), box_(Gtk::ORIENTATION_HORIZONTAL, 0) {
  config_ = config;

  box_.pack_start(image_);
  box_.set_name("image");
  if (!id.empty()) {
    box_.get_style_context()->add_class(id);
  }
  box_.get_style_context()->add_class(module);
  evbox.add(box_);

  size_ = config["size"].asInt();
  if (size_ == 0) {
    size_ = 16;
  }
}

void waybar::modules::image::ImageStrategy::update() { spdlog::info("ImageStrategy update() run"); }
