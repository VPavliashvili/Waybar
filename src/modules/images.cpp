#include "modules/images.hpp"

waybar::modules::Images::Images(const std::string &id, const Json::Value &config)
    : AModule(config, "images", id), box_(Gtk::ORIENTATION_HORIZONTAL, 0) {
  config_ = config;

  box_.set_name("images");
  box_.get_style_context()->add_class(id);
  box_.get_style_context()->add_class(MODULE_CLASS);
  event_box_.add(box_);

  size_ = config["size"].asInt();
  if (size_ == 0) {
    size_ = 16;
  }

  interval_ = config_["interval"].asInt();
  if (interval_ == 0) {
    interval_ = INT_MAX;
  }

  spdlog::info("Images ctor executed");

  delayWorker();
};

void waybar::modules::Images::delayWorker() {
  thread_ = [this] {
    dp.emit();
    auto interval = std::chrono::seconds(interval_);
    thread_.sleep_for(interval);
  };
}

void waybar::modules::Images::refresh(int sig) {
  if (sig == SIGRTMIN + config_["signal"].asInt()) {
    thread_.wake_up();
  }
}

auto waybar::modules::Images::update() -> void {
  spdlog::info("update function run");

  if (box_.get_children().size() > 0) {
    auto children = box_.get_children();
    for (auto child : children) {
      box_.remove(*child);
      spdlog::info("child removed with name -> {}", std::string(child->get_name()));
      delete child;
    }
    entries_.clear();
  }

  if (!config_["entries"].empty()) {
    for (unsigned int i = 0; i < config_["entries"].size(); i++) {
      auto cur = config_["entries"][i];
      if (!cur.isString() || !Glib::file_test(cur.asString(), Glib::FILE_TEST_EXISTS)) {
        spdlog::error("invalid input in images config -> {}", config_["entries"]);
        return;
      }
      auto path = cur.asString();
      entries_.push_back(path);
    }
  } else if (!config_["exec"].empty()) {
    auto exec = util::command::exec(config_["exec"].asString(), "");
    Json::Value as_json;
    Json::Reader reader;

    if (!reader.parse(exec.out, as_json)) {
      spdlog::error("invalid json from exec {}", exec.out);
      return;
    }

    for (unsigned int i = 0; i < as_json.size(); i++) {
      auto cur = as_json[i];
      if (!cur.isString() || !Glib::file_test(cur.asString(), Glib::FILE_TEST_EXISTS)) {
        spdlog::error("invalid entry in exec array -> {}", config_["entries"]);
        return;
      }
      auto path = cur.asString();
      entries_.push_back(path);
    }
  } else {
    spdlog::error("no image files provded in config");
    return;
  }

  for (auto path : entries_) {
    Glib::RefPtr<Gdk::Pixbuf> pixbuf;
    pixbuf = Gdk::Pixbuf::create_from_file(path, size_, size_);
    Gtk::Image *image = new Gtk::Image();
    image->set_name(path);
    box_.pack_start(*image);
    auto name = std::string(image->get_name());
    spdlog::info("box packed with image -> {}, and its size is {}", name,
                 box_.get_children().size());

    if (pixbuf) {
      image->set(pixbuf);
      image->show();
      box_.get_style_context()->remove_class("empty");
    } else {
      image->clear();
      image->hide();
      box_.get_style_context()->add_class("empty");
    }
  }

  // spdlog::info("children count: {}", box_.get_children().size());

  AModule::update();
};
