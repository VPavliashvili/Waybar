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
    spdlog::info("singal received {}", sig);
    thread_.wake_up();
  }
}

auto waybar::modules::Images::update() -> void {
  spdlog::info("update function run");

  // clear box_, previous css classes and memory
  if (box_.get_children().size() > 0) {
    auto children = box_.get_children();
    for (auto child : children) {
      box_.remove(*child);
      spdlog::info("child removed with name -> {}", std::string(child->get_name()));
      delete child;
    }

    // for (auto entry : entries_) {
    //   box_.get_style_context()->remove_class(entry.second);
    // }
    images_data_.clear();
  }

  // set new images from config script
  if (!config_["entries"].empty()) {
    setImagesData(config_["entries"]);
  } else if (!config_["exec"].empty()) {
    auto exec = util::command::exec(config_["exec"].asString(), "");
    Json::Value as_json;
    Json::Reader reader;

    if (!reader.parse(exec.out, as_json)) {
      spdlog::error("invalid json from exec {}", exec.out);
      return;
    }

    setImagesData(as_json);
  } else {
    spdlog::error("no image files provded in config");
    return;
  }

  // draw
  for (auto data : images_data_) {
    auto path = data.path;
    auto status = data.status;

    Glib::RefPtr<Gdk::Pixbuf> pixbuf;
    pixbuf = Gdk::Pixbuf::create_from_file(path, size_, size_);
    Gtk::Image *image = new Gtk::Image();
    image->set_name(path);
    image->get_style_context()->add_class(status);
    image->set_tooltip_text(data.tooltip);
    box_.pack_start(*image);
    auto name = std::string(image->get_name());
    spdlog::info("added image -> {}:{}", status, path);

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

void waybar::modules::Images::setImagesData(const Json::Value &cfg_input) {
  for (unsigned int i = 0; i < cfg_input.size(); i++) {
    auto path = cfg_input[i]["path"];
    auto status = cfg_input[i]["status"];
    auto tooltip = cfg_input[i]["tooltip"];

    bool has_tooltip_err = !tooltip.empty() && !tooltip.isString();

    if (!path.isString() || !status.isString() || has_tooltip_err ||
        !Glib::file_test(path.asString(), Glib::FILE_TEST_EXISTS)) {
      spdlog::error("invalid input in images config -> {}", cfg_input);
      return;
    }
    ImageData data;
    data.path = path.asString();
    data.status = status.asString();
    data.tooltip = !tooltip.empty() ? tooltip.asString() : "";

    images_data_.push_back(data);
  }
}
