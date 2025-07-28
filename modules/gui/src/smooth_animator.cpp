//
// Created by lhy on 25-7-23.
//

#include "sdl_gui/gui/smooth_animator.h"

#include "glog/logging.h"

SmoothAnimator::SmoothAnimator(const float duration, const float final) : Animator(duration), final_(final) {}
bool SmoothAnimator::CheckStartStatus(FloatNumberRef& value) {
  if (!start_status_) {
    start_status_.emplace(value);
    LOG(INFO) << *start_status_;
    return true;
  }
  return false;
}
void SmoothAnimator::Update(FloatNumberRef& value) {
  if (CheckStartStatus(value)) return;
  LOG(INFO) << "?" << GetRate();
  value = *start_status_ + (final_ - *start_status_) * std::min(GetRate(), 1.0f);
}
float SmoothAnimator::get_velocity() const { return (final_ - *start_status_) / *duration_; }
void SmoothAnimator::set_velocity(float velocity) {}
