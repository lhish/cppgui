#include <iostream>

#include "sdl_gui/common/magic_enum.hpp"
#include "sdl_gui/gui/controller.h"
//
// Created by lhy on 25-7-21.
//
int main() {
  auto st = std::chrono::high_resolution_clock::now();
  controller.StartLoop([&]() {
    using std::chrono_literals::operator ""ns;
    for (int i = 0; i < 20; i += 2) {
      const std::optional<UIAttributes> real{UIAttributes{0.08f * static_cast<float>(i / 2 + 1), 0.1, 0.03, 0.03, 1}};
      SDL_Color color_ = {255, 255, 255, 255};
      auto radius_ratio_ = 0;
      controller.DrawRRectShadow(real->x_,
                                 real->y_,
                                 real->w_,
                                 real->h_,
                                 color_,
                                 radius_ratio_,
                                 i);
      controller.DrawRRect(real->x_, real->y_, real->w_, real->h_, color_, radius_ratio_);
      controller.DrawRRectShadow(real->x_,
                                 real->y_ + 0.1,
                                 real->w_ * 2,
                                 real->h_ * 2,
                                 color_,
                                 radius_ratio_,
                                 i);
      controller.DrawRRect(real->x_, real->y_ + 0.1, real->w_ * 2, real->h_ * 2, color_, radius_ratio_);
    }
    auto count = (std::chrono::high_resolution_clock::now() - st) / 1ns / 1e9;
    while (count >= 2) {
      count -= 2;
    }
    for (int i = 0; i < 10; i++) {
      controller.DrawRRectShadow(0.1 * (i + 1),
                                 0.3,
                                 0.07,
                                 0.07,
                                 {255, 255, 255, 255},
                                 0,
                                 (count >= 1 ? 2 - count : count) * 24
      );
      std::cout << count << std::endl;
      count += 1.0 / 12;
      while (count >= 2) {
        count -= 2;
      }
      controller.DrawRRect((i + 1) * 0.1, 0.3, 0.07, 0.07, {255, 255, 255, 255}, 0);
    }
  });
}
