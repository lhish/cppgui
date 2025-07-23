//
// Created by lhy on 25-7-23.
//

#ifndef SMOOTH_ANIMATOR_H
#define SMOOTH_ANIMATOR_H
#include "animator.h"

class SmoothAnimator : public Animator {
 public:
  explicit SmoothAnimator(float duration, float final);

  void Update(float& value) override;

  ~SmoothAnimator() override = default;

 private:
  std::optional<float> start_status_{};
  float final_;
};

#endif  // SMOOTH_ANIMATOR_H
