#include "modules/images.hpp"

#include "ALabel.hpp"

waybar::modules::Images::Images(const std::string &id, const Json::Value &config)
    : ALabel(config, "images", id, "{format}") {
  const auto text = "Hello world";
  label_.set_text(text);
};

auto waybar::modules::Images::update() -> void{/* label_.show(); */};
