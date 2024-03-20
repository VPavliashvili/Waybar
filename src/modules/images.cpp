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

  // so exec script will return images data instead of entries
  if (!config_["exec"].empty()) {
    is_exec_used_ = true;
  } else if (!config_["entries"].empty()) {
    setEntries(config_["entries"]);
    is_exec_used_ = false;
  } else {
    spdlog::error("no image files provded in config");
    return;
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

    for (auto entry : entries_) {
      box_.get_style_context()->remove_class(entry.second);
    }
    entries_.clear();
  }

  // set new images from config script
  if (is_exec_used_) {
    auto exec = util::command::exec(config_["exec"].asString(), "");
    Json::Value as_json;
    Json::Reader reader;

    if (!reader.parse(exec.out, as_json)) {
      spdlog::error("invalid json from exec {}", exec.out);
      return;
    }

    setEntries(as_json);
  }

  // draw
  for (auto entry : entries_) {
    auto path = entry.first;
    auto status = entry.second;

    Glib::RefPtr<Gdk::Pixbuf> pixbuf;
    pixbuf = Gdk::Pixbuf::create_from_file(path, size_, size_);
    Gtk::Image *image = new Gtk::Image();
    image->set_name(path);
    image->get_style_context()->add_class(status);
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

  auto ctx = box_.get_style_context();
  for (auto c : ctx->list_classes()) {
    spdlog::info("has css class -> {}", std::string(c));
  }

  // spdlog::info("children count: {}", box_.get_children().size());

  AModule::update();
};

void waybar::modules::Images::setEntries(const Json::Value &cfg_input) {
  for (unsigned int i = 0; i < cfg_input.size(); i++) {
    auto path = cfg_input[i]["path"];
    auto status = cfg_input[i]["status"];
    if (!path.isString() || !status.isString() ||
        !Glib::file_test(path.asString(), Glib::FILE_TEST_EXISTS)) {
      spdlog::error("invalid input in images config -> {}", cfg_input);
      return;
    }
    entries_.push_back(std::make_pair(path.asString(), status.asString()));
  }
}
