#include "../include/sdl_gui/button.h"

Button::Button(const float x,
               const float y,
               const float w,
               const float h,
               const int depth,
               const SDL_Color &color,
               const float radius_ratio)
  : UI(x, y, w, h, depth),
    color_(color),
    radius_ratio_(radius_ratio) {
}

void Button::Draw(const float offset_x, const float offset_y, const float offset_zoom_rate) {
}