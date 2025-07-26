//
// Created by lhy on 25-7-23.
//

#include "sdl_gui/gui/animation.h"
Animation::Animation(Animation&& other) noexcept
    : value_(other.value_), ui_(std::move(other.ui_)), animator_(std::move(other.animator_)) {}
bool Animation::Update() const {
  if (ui_.expired()) {
    return true;
  }
  animator_->Update(*value_);
  return animator_->IsFinished();
}
Animation& Animation::ChangeAnimator(std::unique_ptr<Animator>&& animator) {
  animator->set_velocity(animator_->get_velocity());
  animator_ = std::move(animator);
  return *this;
}
