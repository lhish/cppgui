#include "sdl_gui/gui/button.h"

#include "sdl_gui/gui/animation.h"
#include "sdl_gui/gui/controller.h"
#include "sdl_gui/gui/smooth_animator.h"

Button::Button(const UIAttributes &attr, std::string name, const SDL_Color &color, const float radius_ratio,
               const int depth)
    : UIGroup(attr, std::move(name), depth, {}), color_(color), radius_ratio_(radius_ratio) {}

void Button::Draw(const UIAttributes &offset) {
  const auto real = controller.CalReal(offset, attr_);
  if (!real) {
    return;
  }
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
  controller.AddTrigger({shared_from_this(),
                         [real, this](const float x, const float y) {
                           // 判断是否在圆角矩形内
                           const auto width = static_cast<float>(controller.GetWidth());
                           const auto radius = std::min(real->w_, real->h_) * radius_ratio_ * width / 2;
                           const UIAttributes real1 = {real->x_ * width, real->y_ * width, real->w_ * width,
                                                       real->h_ * width, real->zoom_rate_};
                           return (x >= real1.x_ && x <= real1.x_ + real1.w_ && y >= real1.y_ + radius &&
                                   y <= real1.y_ + real1.h_ - radius) ||
                                  (x >= real1.x_ + radius && x <= real1.x_ + real1.w_ - radius && y >= real1.y_ &&
                                   y <= real1.y_ + real1.h_) ||
                                  ((x - (real1.x_ + radius)) * (x - (real1.x_ + radius)) +
                                       (y - (real1.y_ + radius)) * (y - (real1.y_ + radius)) <=
                                   radius * radius) ||
                                  ((x - (real1.x_ + real1.w_ - radius)) * (x - (real1.x_ + real1.w_ - radius)) +
                                       (y - (real1.y_ + radius)) * (y - (real1.y_ + radius)) <=
                                   radius * radius) ||
                                  ((x - (real1.x_ + radius)) * (x - (real1.x_ + radius)) +
                                       (y - (real1.y_ + real1.h_ - radius)) * (y - (real1.y_ + real1.h_ - radius)) <=
                                   radius * radius) ||
                                  ((x - (real1.x_ + real1.w_ - radius)) * (x - (real1.x_ + real1.w_ - radius)) +
                                       (y - (real1.y_ + real1.h_ - radius)) * (y - (real1.y_ + real1.h_ - radius)) <=
                                   radius * radius);
                         },
                         [this](float x, float y, const MouseStatus &mouse_status) {
                           if (mouse_status == MouseStatus::MOUSE_LEFT_DOWN) {
                             controller.AddAnimation(
                                 {shared_from_this(), height_, std::make_unique<SmoothAnimator>(1, 10.0f)});
                           }
                         }});
  controller.DrawRRectShadow(real->x_, real->y_, real->w_, real->h_, color_, radius_ratio_, height_);
  controller.DrawRRect(real->x_, real->y_, real->w_, real->h_, color_, radius_ratio_);
  UIGroup::Draw(offset);
}
