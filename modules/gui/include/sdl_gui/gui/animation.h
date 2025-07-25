//
// Created by lhy on 25-7-23.
//

#ifndef ANIMATION_H
#define ANIMATION_H
#include <memory>

#include "animator.h"
#include "ui.h"

class Animation {
 public:
  Animation(const std::weak_ptr<UI>& ui, std::optional<std::reference_wrapper<float>> value,
            std::unique_ptr<Animator>&& animator);
  Animation(Animation&& other) noexcept;
  [[nodiscard]] bool Update() const;
  Animation& ChangeAnimator(std::unique_ptr<Animator>&& animator);
  friend bool operator<(const Animation& lhs, const Animation& rhs) {
    if (lhs.ui_.lock().get() < rhs.ui_.lock().get()) return true;
    if (lhs.value_.has_value() && rhs.value_.has_value()) {
      return &lhs.value_->get() < &rhs.value_->get();
    }
    return lhs.value_ < rhs.value_;
  }
  friend bool operator<=(const Animation& lhs, const Animation& rhs) { return !(rhs < lhs); }
  friend bool operator>(const Animation& lhs, const Animation& rhs) { return rhs < lhs; }
  friend bool operator>=(const Animation& lhs, const Animation& rhs) { return !(lhs < rhs); }
  friend bool operator==(const Animation& lhs, const Animation& rhs) {
    return &lhs.value_->get() == &rhs.value_->get();
  }
  friend bool operator!=(const Animation& lhs, const Animation& rhs) { return !(lhs == rhs); }
  std::optional<std::reference_wrapper<float>> value_;
  std::weak_ptr<UI> ui_;
  std::unique_ptr<Animator> animator_;
};

#endif  // ANIMATION_H
