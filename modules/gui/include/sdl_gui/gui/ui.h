#ifndef SDL_GUI_UI_H
#define SDL_GUI_UI_H
#include <cassert>

#include "ui_ref.h"
#include "sdl_gui/common/debug.h"

struct UIAttributes {
  float x_{}; //0-1 x_*width
  float y_{}; //>0 y_*width
  float w_{}; //0-1 w_*width
  float h_{}; //>0(ratio) h_*w_*width
  float zoom_rate_{1};
};

class UI {
  friend class UIRef;
  friend class Controller;

  friend bool operator<(const UIRef &lhs, const UIRef &rhs);

  public:
    virtual ~UI() = default;

    UI(const UIAttributes &attr, std::string name, int depth);

    virtual void Draw(const UIAttributes &offset) {
    }

    [[nodiscard]] std::string getname() const {
      return name_;
    }

    virtual void AddObject(const UIRef &ref) { assertm(false, "only basic ui can't be as parent"); }

    virtual void Click();

    void SetUIRef(const UIRef &ref);

    friend bool operator==(const UI &lhs, const UI &rhs);

    friend bool operator!=(const UI &lhs, const UI &rhs);

    friend bool operator<(const UI &lhs, const UI &rhs);

    friend bool operator<=(const UI &lhs, const UI &rhs);

    friend bool operator>(const UI &lhs, const UI &rhs);

    friend bool operator>=(const UI &lhs, const UI &rhs);

  protected:
    UIAttributes attr_{};
    int depth_{}; //>=0
    UIRef ref_{};
    std::string name_;
};

#endif //SDL_GUI_UI_H
