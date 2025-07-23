#ifndef SDL_GUI_UI_H
#define SDL_GUI_UI_H
#include <cassert>
#include <memory>

#include "sdl_gui/common/debug.h"
#include "ui_factory.h"

struct UIAttributes {
  float x_{};  // 0-1 x_*width
  float y_{};  //>0 y_*width
  float w_{};  // 0-1 w_*width
  float h_{};  //>0(ratio) h_*w_*width
  float zoom_rate_{1};
};

class UI : public std::enable_shared_from_this<UI> {
  friend class UIRef;
  friend class Controller;

  friend bool operator<(const std::shared_ptr<UI> &lhs, const std::shared_ptr<UI> &rhs) {
    return lhs->depth_ < rhs->depth_;
  }

 public:
  virtual ~UI() = default;

  UI(const UIAttributes &attr, std::string name, int depth);

  virtual void Draw(const UIAttributes &offset) {}

  [[nodiscard]] std::string getname() const { return name_; }

  virtual void AddObject(const std::shared_ptr<UI> &ref) { assertm(false, "only basic ui can't be as parent"); }

  virtual void Click();

  friend bool operator==(const UI &lhs, const UI &rhs);

  friend bool operator!=(const UI &lhs, const UI &rhs);

  friend bool operator<(const UI &lhs, const UI &rhs);

  friend bool operator<=(const UI &lhs, const UI &rhs);

  friend bool operator>(const UI &lhs, const UI &rhs);

  friend bool operator>=(const UI &lhs, const UI &rhs);

 protected:
  UIAttributes attr_{};
  int depth_{};  //>=0
  std::string name_;
  int index_;
  inline static int counter_{};
};

template <>
struct std::less<std::shared_ptr<UI> > {
  bool operator()(const std::shared_ptr<UI> &lhs, const std::shared_ptr<UI> &rhs) const { return *lhs < *rhs; }
};

UI_FACTORY_REGISTER(UIType::UI, UI)
#endif  // SDL_GUI_UI_H
