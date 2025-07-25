//
// Created by lhy on 25-7-23.
//

#include "sdl_gui/gui/animator.h"

Animator::Animator(const std::optional<float> duration)
    : start_time_(std::chrono::high_resolution_clock::now()), duration_(duration) {}

float Animator::GetRate() const {
  if (!duration_) {
    return 0;
  }
  using std::chrono_literals::operator""ns;
  return static_cast<float>((std::chrono::high_resolution_clock::now() - start_time_) / 1e9ns) / *duration_;
}
bool Animator::IsFinished() const { return GetRate() >= 1; }
