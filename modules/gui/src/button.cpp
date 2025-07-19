#include "sdl_gui/gui/button.h"
#include "sdl_gui/gui/controller.h"

Button::Button(const float x,
               const float y_ratio,
               const float w,
               const float h_ratio,
               const int depth,
               const SDL_Color &color,
               const float radius_ratio
)
  : UI(x, y_ratio, w, h_ratio, depth),
    color_(color),
    radius_ratio_(radius_ratio) {
}

void Button::Draw(const float offset_x, const float offset_y, const float offset_zoom_rate) {
  const float final_zoom_rate = offset_zoom_rate * zoom_rate_;
  const float abs_x = x_ * final_zoom_rate + offset_x;
  const float abs_y = y_ratio_ * x_ * final_zoom_rate + offset_y;
  const float abs_w = w_ * final_zoom_rate;
  const float abs_h = w_ * h_ratio_ * final_zoom_rate;
  // SkScalar ambient_elevation = 4.0f;
  // SkScalar spot_elevation = 6.0f;

  // if (ambient_elevation > 0 || spot_elevation > 0) {
  // SkPath path;
  // path.addRRect(rrect);
  // SkShadowUtils::DrawShadow(canvas,
  // path,
  // SkPoint3::Make(0, 0, ambient_elevation),
  // SkPoint3::Make(0, 0, spot_elevation),
  // SkColorSetARGB(255, 0, 0, 0),
  // SkColorSetARGB(128, 0, 0, 0),
  // SkShadowFlags::kNone_ShadowFlag);
  // }

  // Draw background
  controller.DrawRRect(abs_x, abs_y, abs_w, abs_h, color_, radius_ratio_);
}
