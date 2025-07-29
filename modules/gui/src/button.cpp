#include "sdl_gui/gui/button.h"

#include "sdl_gui/gui/animation.h"
#include "sdl_gui/gui/controller.h"
#include "sdl_gui/gui/smooth_animator.h"
#include "sdl_gui/gui/spring_animator.h"

Button::Button(const UIAttributes &attr, std::string name, const SDL_Color &color, const float radius_ratio,
               const int depth)
    : Shape(attr, std::move(name), color, radius_ratio, depth) {
  circle_ =
      *ui_group_.emplace(UIFactory::CreateUI<UIType::Shape>(attr, name + "'s circle", SDL_Color{255, 255, 255, 255}, 1))
           .first;
  circle_.lock()->set_visibility(false);
}
void Button::DrawKids() {
  controller.SaveCanvas();
  controller.AddClipRRect(real_.x_, real_.y_, real_.w_, real_.h_, radius_ratio_);
  Shape::DrawKids();
  controller.RestoreCanvas();
}
void Button::DrawSelf() {
  Shape::DrawSelf();
  controller.DrawRRectShadow(real_.x_, real_.y_, real_.w_, real_.h_, color_, radius_ratio_, height_);
}
void Button::AddTrigger() {
  // todo: make it only work when animating
  controller.AddTrigger(
      shared_from_this(),
      {[this](const float x, const float y) {
         // 判断是否在圆角矩形内
         const auto width = static_cast<float>(controller.GetWidth());
         const auto radius = std::min(real_.w_, real_.h_) * radius_ratio_ * width / 2;
         const UIAttributes real1 = {real_.x_ * width, real_.y_ * width, real_.w_ * width, real_.h_ * width,
                                     real_.zoom_rate_};
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
               {SpringAnimator::Create(2, SpringCategory::ExpressiveEffectsFast), shared_from_this(), height_});
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
           controller.AddAnimation({std::make_unique<SmoothAnimator>(0.2, 1), shared_from_this(), height_});
         }
         auto &circle_color = lhy::GetMember<SDL_Color>(*std::dynamic_pointer_cast<Shape>(circle_.lock()), "color_");
         if (mouse_status == MouseInteraction::StartLeftClick) {
           controller.AddAnimation({std::make_unique<SmoothAnimator>(0.2, 8), shared_from_this(), height_});
           auto [mouse_x, mouse_y] = controller.MousePositionChange(shared_from_this(), x, y);
           auto &[circle_x, circle_y, circle_w, circle_h, circle_zoom] =
               lhy::GetMember<UIAttributes>(*circle_.lock(), "attr_");
           circle_x = mouse_x;
           circle_y = mouse_y;
           circle_w = 0;
           circle_h = 1;
           circle_.lock()->set_visibility(true);
           circle_color = {255, 255, 255, 0};
           const auto final_w = attr_.w_ * 2;
           controller.AddAnimation(
               {std::make_unique<SmoothAnimator>(1, circle_x - final_w / 2), shared_from_this(), circle_x});
           controller.AddAnimation(
               {std::make_unique<SmoothAnimator>(1, circle_y - final_w / 2), shared_from_this(), circle_y});
           controller.AddAnimation({std::make_unique<SmoothAnimator>(1, final_w), shared_from_this(), circle_w});
           controller.AddAnimation({std::make_unique<SmoothAnimator>(1, 80), shared_from_this(), circle_color.a});
         }
         if (mouse_status == MouseInteraction::StopLeftClick) {
           controller.AddAnimation({std::make_unique<SmoothAnimator>(0.2, 1), shared_from_this(), height_});
           controller.AddAnimation(
               {SpringAnimator::Create(0, SpringCategory::ExpressiveEffectsFast), shared_from_this(), circle_color.a});
         }
       }});
}
