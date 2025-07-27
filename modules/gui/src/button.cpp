#include "sdl_gui/gui/button.h"

#include "sdl_gui/gui/animation.h"
#include "sdl_gui/gui/controller.h"
#include "sdl_gui/gui/spring_animator.h"

Button::Button(const UIAttributes &attr, std::string name, const SDL_Color &color, const float radius_ratio,
               const int depth)
    : UIGroup(attr, std::move(name), depth, {}), color_(color), radius_ratio_(radius_ratio) {}

void Button::Draw(const UIAttributes &offset) {
  real_ = controller.CalReal(offset, attr_);
  if (!real_) {
    return;
  }
  AddTrigger(real_);
  controller.DrawRRectShadow(real_->x_, real_->y_, real_->w_, real_->h_, color_, radius_ratio_, height_);
  controller.DrawRRect(real_->x_, real_->y_, real_->w_, real_->h_, color_, radius_ratio_);
  UIGroup::Draw(offset);
}
void Button::AddTrigger(const std::optional<UIAttributes> &real) {
  // todo: make it only work when animating
  controller.AddTrigger(
      shared_from_this(),
      {[real, this](const float x, const float y) {
         // 判断是否在圆角矩形内
         const auto width = static_cast<float>(controller.GetWidth());
         const auto radius = std::min(real->w_, real->h_) * radius_ratio_ * width / 2;
         const UIAttributes real1 = {real->x_ * width, real->y_ * width, real->w_ * width, real->h_ * width,
                                     real->zoom_rate_};
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
       [this](const float x, const float y, const MouseInteraction &mouse_status) {
         if (mouse_status == MouseInteraction::StartHover) {
           controller.Store(color_.r);
           controller.Store(color_.g);
           controller.Store(color_.b);
           controller.AddAnimation(
               {SpringAnimator::Create(std::max(0, color_.r - 10), SpringCategory::ExpressiveEffectsFast),
                shared_from_this(), color_.r});
           controller.AddAnimation(
               {SpringAnimator::Create(std::max(0, color_.g - 10), SpringCategory::ExpressiveEffectsFast),
                shared_from_this(), color_.g});
           controller.AddAnimation(
               {SpringAnimator::Create(std::max(0, color_.b - 10), SpringCategory::ExpressiveEffectsFast),
                shared_from_this(), color_.b});
         }
         if (mouse_status == MouseInteraction::LeaveAreaHover || mouse_status == MouseInteraction::LeaveAreaLeftClick ||
             mouse_status == MouseInteraction::LeaveAreaRightClick) {
           controller.AddAnimation(
               {SpringAnimator::Create(controller.WithDraw(color_.r), SpringCategory::ExpressiveEffectsFast),
                shared_from_this(), color_.r});
           controller.AddAnimation(
               {SpringAnimator::Create(controller.WithDraw(color_.g), SpringCategory::ExpressiveEffectsFast),
                shared_from_this(), color_.g});
           controller.AddAnimation(
               {SpringAnimator::Create(controller.WithDraw(color_.b), SpringCategory::ExpressiveEffectsFast),
                shared_from_this(), color_.b});
         }
         if (mouse_status == MouseInteraction::StartLeftClick) {
           auto [x1, y1] = controller.MousePositionChange(shared_from_this(), x, y);
           controller.AddAnimation({
               SpringAnimator::Create(attr_.x_ + 0.4f, SpringCategory::ExpressiveSpatialFast),
               shared_from_this(),
               attr_.x_,
           });
         }
       }});
}
