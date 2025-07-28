#ifndef SDL_GUI_UI_H
#define SDL_GUI_UI_H
#include <cassert>
#include <memory>

#include "sdl_gui/common/debug.h"
#include "sdl_gui/common/refl_s.h"
#include "ui_factory.h"

struct UIAttributes {
  float x_{};           // 0-1 x_*width  //相较于父元素的偏移量，值的单位是width
  float y_{};           //>0 y_*width    //相较于父元素的偏移量，值的单位是width
  float w_{};           // 0-1 w_*width  //宽度，值的单位是width
  float h_{};           //>0(ratio) h_*w_*width   //高度，值的单位是w*width
  float zoom_rate_{1};  // 通过zoom来进行整体调整
};

class UI : public std::enable_shared_from_this<UI> {
  friend class UIRef;
  friend class Controller;

  friend bool operator<(const std::shared_ptr<UI> &lhs, const std::shared_ptr<UI> &rhs) {
    return lhs->depth_ < rhs->depth_;
  }

 public:
  static constexpr float from_bottom_ = 1000;
  static float FromBottomY(float y);
  UI() {}
  virtual ~UI() = default;

  UI(const UIAttributes &attr, std::string name, int depth);

  void Draw(const UIAttributes &offset);
  bool CalReal(const UIAttributes &offset);

  virtual void DrawReal() {};

  [[nodiscard]] std::string getname() const { return name_; }

  virtual void AddObject(const std::shared_ptr<UI> &ref) { assertm(false, "only basic ui can't be as parent"); }

  virtual void Click();

  virtual void AddTrigger();

  friend bool operator==(const UI &lhs, const UI &rhs);

  friend bool operator!=(const UI &lhs, const UI &rhs);

  friend bool operator<(const UI &lhs, const UI &rhs);

  friend bool operator<=(const UI &lhs, const UI &rhs);

  friend bool operator>(const UI &lhs, const UI &rhs);

  friend bool operator>=(const UI &lhs, const UI &rhs);

  const UIAttributes &get_real();

  void set_visibility(bool visibility);

 protected:
  UIAttributes attr_{};
  int depth_{};  //>=0
  std::string name_;
  int index_;
  inline static int counter_{};
  std::optional<UIAttributes> real_{};
  bool visibility_{true};
};
REFL(UI, attr_)
template <>
struct std::less<std::shared_ptr<UI> > {
  constexpr bool operator()(const std::shared_ptr<UI> &lhs, const std::shared_ptr<UI> &rhs) const {
    return *lhs < *rhs;
  }
};
template <>
struct std::less<std::weak_ptr<UI> > {
  constexpr bool operator()(const std::weak_ptr<UI> &lhs, const std::weak_ptr<UI> &rhs) const {
    if (!lhs.expired() && !rhs.expired()) {
      return *lhs.lock() < *rhs.lock();
    }
    return lhs.lock().get() < rhs.lock().get();
  }
};
UI_FACTORY_REGISTER(UIType::UI, UI)
#endif  // SDL_GUI_UI_H
