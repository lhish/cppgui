//
// Created by lhy on 25-7-26.
//

#ifndef FLOAT_NUMBER_REF_H
#define FLOAT_NUMBER_REF_H
#include <any>
#include <functional>

class FloatNumberRef {
 public:
  friend bool operator<(const FloatNumberRef& lhs, const FloatNumberRef& rhs) { return lhs.get() < rhs.get(); }
  friend bool operator<=(const FloatNumberRef& lhs, const FloatNumberRef& rhs) { return !(rhs < lhs); }
  friend bool operator>(const FloatNumberRef& lhs, const FloatNumberRef& rhs) { return rhs < lhs; }
  friend bool operator>=(const FloatNumberRef& lhs, const FloatNumberRef& rhs) { return !(lhs < rhs); }
  friend bool operator==(const FloatNumberRef& lhs, const FloatNumberRef& rhs) { return lhs.get() == rhs.get(); }
  friend bool operator!=(const FloatNumberRef& lhs, const FloatNumberRef& rhs) { return !(lhs == rhs); }
  FloatNumberRef& operator=(const float& other) {
    SetFloat(other);
    return *this;
  }
  virtual float GetFloat() const = 0;
  virtual void SetFloat(float) = 0;
  operator float() const { return GetFloat(); }
  virtual ~FloatNumberRef() = default;
  virtual void* get() const = 0;
  virtual bool has_value() = 0;
  virtual std::any GetAny() = 0;
};
template <typename T>
class FloatNumberRefT : public FloatNumberRef {
 public:
  FloatNumberRefT(T& value) : value_(value) {}
  void* get() const override { return &value_->get(); }
  ~FloatNumberRefT() override = default;
  FloatNumberRefT(const FloatNumberRefT& other) : value_(other.value_) {}
  FloatNumberRefT& operator=(const FloatNumberRefT& other) {
    if (this == &other) return *this;
    value_ = other.value_;
    return *this;
  }
  float GetFloat() const override { return static_cast<float>(*value_); }
  void SetFloat(const float other) override { value_->get() = static_cast<T>(other); }
  bool has_value() override { return value_.has_value(); }
  std::any GetAny() override { return *value_; }

 private:
  std::optional<std::reference_wrapper<T>> value_;
};
template <>
struct std::less<std::shared_ptr<FloatNumberRef>> {
  constexpr bool operator()(const std::shared_ptr<FloatNumberRef>& lhs,
                            const std::shared_ptr<FloatNumberRef>& rhs) const {
    return *lhs < *rhs;
  }
};
#endif  // FLOAT_NUMBER_REF_H
