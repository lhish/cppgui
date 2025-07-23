//
// Created by lhy on 25-7-23.
//

#include "sdl_gui/gui/smooth_animator.h"

#include "glog/logging.h"

SmoothAnimator::SmoothAnimator(const float duration, const float final) : Animator(duration), final_(final) {}
void SmoothAnimator::Update(float& value) {
  if (!start_status_) {
    start_status_ = value;
    return;
  }
  value = *start_status_ + (final_ - *start_status_) * GetRate();
  LOG(INFO) << GetRate();
}
