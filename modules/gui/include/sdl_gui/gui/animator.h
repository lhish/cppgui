//
// Created by lhy on 25-7-23.
//

#ifndef ANIMATOR_H
#define ANIMATOR_H
#include <chrono>

class Animator {
 public:
  explicit Animator(std::optional<float> duration);

  [[nodiscard]] float GetRate() const;

  [[nodiscard]] virtual bool IsFinished() const;

  virtual void Update(float& value) = 0;

  virtual ~Animator() = default;

  [[nodiscard]] virtual float get_velocity() const;
  virtual void set_velocity(float velocity);

 protected:
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time_;
  std::optional<float> duration_;
  float velocity_{};
};

#endif  // ANIMATOR_H
