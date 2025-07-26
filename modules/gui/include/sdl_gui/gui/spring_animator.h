#ifndef SPRING_ANIMATOR_H
#define SPRING_ANIMATOR_H

#include <functional>

#include "animator.h"
enum class SpringCategory {
  StandardSpatialFast,
  StandardSpatialDefault,
  StandardSpatialSlow,
  StandardEffectsFast,
  StandardEffectsDefault,
  StandardEffectsSlow,
  ExpressiveSpatialFast,
  ExpressiveSpatialDefault,
  ExpressiveSpatialSlow,
  ExpressiveEffectsFast,
  ExpressiveEffectsDefault,
  ExpressiveEffectsSlow
};
class SpringAnimator : public Animator {
 public:
  static std::unique_ptr<Animator> Create(float final, SpringCategory spring_category,
                                          std::optional<float> duration = {});
  // recommend not use duration
  [[deprecated("should use Create")]]
  explicit SpringAnimator(float final, float damping_ratio, float stiffness, std::optional<float> duration = {});
  ~SpringAnimator() override = default;
  void Update(FloatNumberRef& value) override;

 private:
  [[nodiscard]] constexpr float EstimateTime() const;

  static constexpr float EstimateAnimationDuration(double stiffness, double damping_ratio, double initial_velocity,
                                                   double initial_displacement, double delta);
  static constexpr double EstimateUnderDamped(double first_root_real, double first_root_imaginary, double p0, double v0,
                                              double delta);
  static constexpr double IterateNewtonsMethod(double x, const std::function<double(double)>& fn,
                                               const std::function<double(double)>& fn_prime);
  static constexpr double EstimateCriticallyDamped(double first_root_real, double p0, double v0, double delta);
  static constexpr double EstimateOverDamped(double first_root_real, double second_root_real, double p0, double v0,
                                             double delta);
  static constexpr float EstimateDurationInternal(double first_root_real, double first_root_imaginary,
                                                  double second_root_real, double damping_ratio,
                                                  double initial_velocity, double initial_position, double delta);
  float final_;
  std::optional<float> start_status_{};
  float stiffness_;
  float damping_ratio_;
  float visibility_threshold_{0.01f};
  float last_s_{};
  float ratio_{};
  std::chrono::high_resolution_clock::time_point last_t_{};
  inline static const std::unordered_map<SpringCategory, std::pair<float, float> > spring_parameters{
      {SpringCategory::StandardSpatialFast, {0.9, 1400}},    {SpringCategory::StandardSpatialDefault, {0.9, 700}},
      {SpringCategory::StandardSpatialSlow, {0.9, 300}},     {SpringCategory::StandardEffectsFast, {1, 3800}},
      {SpringCategory::StandardEffectsDefault, {1, 1600}},   {SpringCategory::StandardEffectsSlow, {1, 800}},
      {SpringCategory::ExpressiveSpatialFast, {0.6, 800}},   {SpringCategory::ExpressiveSpatialDefault, {0.8, 380}},
      {SpringCategory::ExpressiveSpatialSlow, {0.8, 200}},   {SpringCategory::ExpressiveEffectsFast, {1, 3800}},
      {SpringCategory::ExpressiveEffectsDefault, {1, 1600}}, {SpringCategory::ExpressiveEffectsSlow, {1, 800}}};
};

#endif  // SPRING_ANIMATOR_H