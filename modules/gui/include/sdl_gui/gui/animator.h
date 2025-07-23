//
// Created by lhy on 25-7-23.
//

#ifndef ANIMATOR_H
#define ANIMATOR_H
#include <chrono>

class Animator {
 public:
  explicit Animator(float duration);

  float GetRate() const;

  [[nodiscard]] bool IsFinished() const;

  virtual void Update(float& value) = 0;

  virtual ~Animator() = default;

 private:
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time_;
  float duration_;
};

#endif  // ANIMATOR_H
