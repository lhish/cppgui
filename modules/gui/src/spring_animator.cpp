#include "sdl_gui/gui/spring_animator.h"

#include <cassert>
#include <chrono>
#include <functional>

#include "sdl_gui/common/debug.h"

SpringAnimator::SpringAnimator(const float final, const float damping_ratio, const float stiffness,
                               const std::optional<float> duration)
    : Animator(duration), final_(final), stiffness_(stiffness), damping_ratio_(damping_ratio) {
  assertm(damping_ratio != 0, "damping_ratio cannot be zero");
}
constexpr float SpringAnimator::EstimateTime() const {
  return EstimateAnimationDuration(stiffness_, damping_ratio_, 0, (*start_status_ - final_) / visibility_threshold_, 1);
}
constexpr float SpringAnimator::EstimateAnimationDuration(const double stiffness, const double damping_ratio,
                                                          const double initial_velocity,
                                                          const double initial_displacement, const double delta) {
  const double dampingCoefficient = 2.0 * damping_ratio * std::sqrt(stiffness);

  const double partialRoot = dampingCoefficient * dampingCoefficient - 4.0 * stiffness;
  const double partialRootReal = (partialRoot < 0.0) ? 0.0 : std::sqrt(partialRoot);
  const double partialRootImaginary = (partialRoot < 0.0) ? std::sqrt(std::abs(partialRoot)) : 0.0;

  const double firstRootReal = (-dampingCoefficient + partialRootReal) * 0.5;
  const double firstRootImaginary = partialRootImaginary * 0.5;
  const double secondRootReal = (-dampingCoefficient - partialRootReal) * 0.5;

  return EstimateDurationInternal(firstRootReal, firstRootImaginary, secondRootReal, damping_ratio, initial_velocity,
                                  initial_displacement, delta);
}
constexpr double SpringAnimator::EstimateUnderDamped(const double first_root_real, const double first_root_imaginary,
                                                     const double p0, const double v0, const double delta) {
  const double r = first_root_real;
  const double c1 = p0;
  const double c2 = (v0 - r * c1) / first_root_imaginary;
  const double c = std::sqrt(c1 * c1 + c2 * c2);

  return std::log(delta / c) / r;
}
constexpr double SpringAnimator::IterateNewtonsMethod(const double x, const std::function<double(double)>& fn,
                                                      const std::function<double(double)>& fn_prime) {
  return x - fn(x) / fn_prime(x);
}

constexpr double SpringAnimator::EstimateCriticallyDamped(const double first_root_real, const double p0,
                                                          const double v0, const double delta) {
  const double r = first_root_real;
  const double c1 = p0;
  const double c2 = v0 - r * c1;

  const double t1 = std::log(std::abs(delta / c1)) / r;
  const double t2 = [&]() {
    const double guess = std::log(std::abs(delta / c2));
    double t = guess;
    for (int i = 0; i < 6; ++i) {
      t = (guess - std::log(std::abs(t / r)));
    }
    return t;
  }() / r;

  double tCurr;
  if (std::isnan(t1) || !std::isfinite(t1)) {
    tCurr = t2;
  } else if (std::isnan(t2) || !std::isfinite(t2)) {
    tCurr = t1;
  } else {
    tCurr = std::max(t1, t2);
  }

  const double tInflection = -(r * c1 + c2) / (r * c2);
  const double xInflection = c1 * std::exp(r * tInflection) + c2 * tInflection * std::exp(r * tInflection);

  double signedDelta;
  if (std::isnan(tInflection) || tInflection <= 0.0) {
    signedDelta = -delta;
  } else if (tInflection > 0.0 && -xInflection < delta) {
    if (c2 < 0 && c1 > 0) {
      tCurr = 0.0;
    }
    signedDelta = -delta;
  } else {
    tCurr = -(2.0 / r) - (c1 / c2);
    signedDelta = delta;
  }

  double tDelta = std::numeric_limits<double>::max();
  int iterations = 0;
  while (tDelta > 0.001 && iterations < 100) {
    iterations++;
    const double tLast = tCurr;
    tCurr = IterateNewtonsMethod(
        tCurr, [&](double t) { return (c1 + c2 * t) * std::exp(r * t) + signedDelta; },
        [&](double t) { return (c2 * (r * t + 1) + c1 * r) * std::exp(r * t); });
    tDelta = std::abs(tLast - tCurr);
  }

  return tCurr;
}

constexpr double SpringAnimator::EstimateOverDamped(const double first_root_real, const double second_root_real,
                                                    const double p0, const double v0, const double delta) {
  const double r1 = first_root_real;
  const double r2 = second_root_real;
  const double c2 = (r1 * p0 - v0) / (r1 - r2);
  const double c1 = p0 - c2;

  const double t1 = std::log(std::abs(delta / c1)) / r1;
  const double t2 = std::log(std::abs(delta / c2)) / r2;

  double tCurr;
  if (std::isnan(t1) || !std::isfinite(t1)) {
    tCurr = t2;
  } else if (std::isnan(t2) || !std::isfinite(t2)) {
    tCurr = t1;
  } else {
    tCurr = std::max(t1, t2);
  }

  const double tInflection = std::log((c1 * r1) / (-c2 * r2)) / (r2 - r1);
  auto xInflection = [&]() { return c1 * std::exp(r1 * tInflection) + c2 * std::exp(r2 * tInflection); };

  double signedDelta;
  if (std::isnan(tInflection) || tInflection <= 0.0) {
    signedDelta = -delta;
  } else if (tInflection > 0.0 && -xInflection() < delta) {
    if (c2 > 0.0 && c1 < 0.0) {
      tCurr = 0.0;
    }
    signedDelta = -delta;
  } else {
    tCurr = std::log(-(c2 * r2 * r2) / (c1 * r1 * r1)) / (r1 - r2);
    signedDelta = delta;
  }

  if (std::abs(c1 * r1 * std::exp(r1 * tCurr) + c2 * r2 * std::exp(r2 * tCurr)) < 0.0001) {
    return tCurr;
  }

  double tDelta = std::numeric_limits<double>::max();
  int iterations = 0;
  while (tDelta > 0.001 && iterations < 100) {
    iterations++;
    const double tLast = tCurr;
    tCurr = IterateNewtonsMethod(
        tCurr, [&](double t) { return c1 * std::exp(r1 * t) + c2 * std::exp(r2 * t) + signedDelta; },
        [&](double t) { return c1 * r1 * std::exp(r1 * t) + c2 * r2 * std::exp(r2 * t); });
    tDelta = std::abs(tLast - tCurr);
  }

  return tCurr;
}

constexpr float SpringAnimator::EstimateDurationInternal(const double first_root_real,
                                                         const double first_root_imaginary,
                                                         const double second_root_real, const double damping_ratio,
                                                         const double initial_velocity, const double initial_position,
                                                         const double delta) {
  const double v0 = (initial_position < 0) ? -initial_velocity : initial_velocity;
  const double p0 = std::abs(initial_position);

  double estimatedTime;
  if (damping_ratio > 1.0) {
    estimatedTime = EstimateOverDamped(first_root_real, second_root_real, p0, v0, delta);
  } else if (damping_ratio < 1.0) {
    estimatedTime = EstimateUnderDamped(first_root_real, first_root_imaginary, p0, v0, delta);
  } else {
    estimatedTime = EstimateCriticallyDamped(first_root_real, p0, v0, delta);
  }

  return static_cast<float>(estimatedTime);
}

std::unique_ptr<Animator> SpringAnimator::Create(float final, SpringCategory spring_category,
                                                 std::optional<float> duration) {
  auto& [damping_ratio, stiffness] = spring_parameters.at(spring_category);
  return std::make_unique<SpringAnimator>(final, damping_ratio, stiffness, duration);
}
void SpringAnimator::Update(float& value) {
  if (!start_status_) {
    start_status_ = value;
    last_s_ = value;
    last_t_ = std::chrono::high_resolution_clock::now();
    if (!duration_) {
      duration_ = EstimateTime();
      ratio_ = 1;
    }
    ratio_ = *duration_ / EstimateTime();
    return;
  }
  const float adjusted_displacement = last_s_ - final_;
  using std::chrono_literals::operator""ns;
  const double delta_t =
      static_cast<double>((std::chrono::high_resolution_clock::now() - last_t_) / 1ns) / 1e9 / ratio_;  // s
  const double damping_ratio_squared = damping_ratio_ * damping_ratio_;
  const double natural_freq = std::sqrt(stiffness_);
  const double r = -damping_ratio_ * natural_freq;

  double displacement;
  double current_velocity;

  if (damping_ratio_ > 1) {
    const double s = natural_freq * std::sqrt(damping_ratio_squared - 1);
    const double gamma_plus = r + s;
    const double gamma_minus = r - s;

    const double co_eff_b = (gamma_minus * adjusted_displacement - velocity_) / (gamma_minus - gamma_plus);
    const double co_eff_a = adjusted_displacement - co_eff_b;
    displacement = co_eff_a * std::exp(gamma_minus * delta_t) + co_eff_b * std::exp(gamma_plus * delta_t);
    current_velocity = co_eff_a * gamma_minus * std::exp(gamma_minus * delta_t) +
                       co_eff_b * gamma_plus * std::exp(gamma_plus * delta_t);
  } else if (damping_ratio_ == 1.0f) {
    const double co_eff_a = adjusted_displacement;
    const double co_eff_b = velocity_ + natural_freq * adjusted_displacement;
    const double nfd_t = -natural_freq * delta_t;
    displacement = (co_eff_a + co_eff_b * delta_t) * std::exp(nfd_t);
    current_velocity = (co_eff_a + co_eff_b * delta_t) * std::exp(nfd_t) * -natural_freq + co_eff_b * std::exp(nfd_t);
  } else {
    const double damped_freq = natural_freq * std::sqrt(1 - damping_ratio_squared);
    const double cos_co_eff = adjusted_displacement;
    const double sin_co_eff = 1 / damped_freq * (-r * adjusted_displacement + velocity_);
    const double dfd_t = damped_freq * delta_t;
    displacement = std::exp(r * delta_t) * (cos_co_eff * std::cos(dfd_t) + sin_co_eff * std::sin(dfd_t));
    current_velocity = displacement * r + std::exp(r * delta_t) * (-damped_freq * cos_co_eff * std::sin(dfd_t) +
                                                                   damped_freq * sin_co_eff * std::cos(dfd_t));
  }

  velocity_ = static_cast<float>(current_velocity);

  last_t_ = std::chrono::high_resolution_clock::now();
  value = last_s_ = static_cast<float>(displacement) + final_;
}