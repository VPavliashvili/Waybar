#pragma once

#include "ALabel.hpp"

namespace waybar::modules {

class Images : public ALabel {
  public:
  Images(const std::string&, const Json::Value&);
  virtual ~Images() = default;
  auto update() -> void override;
};

}  // namespace waybar::modules
