#include "sdl_gui/gui/button.h"
#include "sdl_gui/gui/controller.h"

Button::Button(const UIAttributes &attr,
               const int depth,
               const SDL_Color &color,
               const float radius_ratio
)
  : UIGroup(attr, depth, {}),
    color_(color),
    radius_ratio_(radius_ratio) {
}

void Button::Draw(const UIAttributes &offset) {
  const auto real = Controller::CalReal(offset, attr_);
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
  controller.DrawRRect(real.x_, real.y_, real.w_, real.h_, color_, radius_ratio_);
  UIGroup::Draw(offset);
}
