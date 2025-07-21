#include "sdl_gui/common/magic_enum.hpp"
#include "sdl_gui/gui/controller.h"
//
// Created by lhy on 25-7-21.
//
int main() {
  constexpr auto colors = magic_enum::enum_values<ShadowHeights>();
  controller.StartLoop([&]() {
    for (int i = 0; i < colors.size(); i++) {
      const std::optional<UIAttributes> real{UIAttributes{0.1f * static_cast<float>(i + 1), 0.1, 0.07, 0.07, 1}};
      SDL_Color color_ = {255, 255, 255, 255};
      auto radius_ratio_ = 0;
      controller.DrawRRectShadow(real->x_,
                                 real->y_,
                                 real->w_,
                                 real->h_,
                                 color_,
                                 radius_ratio_,
                                 colors[i]);
      controller.DrawRRect(real->x_, real->y_, real->w_, real->h_, color_, radius_ratio_);
    }
  });
}
