#ifndef SDL_GUI_UI_H
#define SDL_GUI_UI_H
#include <cassert>

#include "sdl_gui/common/debug.h"

class UIRef;

struct UIAttributes {
  float x_{}; //0-1 x_*width
  float y_{}; //>0 y_*width
  float w_{}; //0-1 w_*width
  float h_{}; //>0(ratio) h_*w_*width
  float zoom_rate_{1};
};

class UI {
  friend class UIRef;

  friend bool operator<(const UIRef &lhs, const UIRef &rhs);

  public:
    virtual ~UI() = default;

    explicit UI() = default;

    UI(UIAttributes attr, int depth);

    virtual void Draw(const UIAttributes &offset) {
    }

    virtual void AddObject(const UIRef &ref) { assertm(false, "only basic ui can't be as parent"); }

    virtual void Click();

    friend bool operator==(const UI &lhs, const UI &rhs);

    friend bool operator!=(const UI &lhs, const UI &rhs);

    friend bool operator<(const UI &lhs, const UI &rhs);

    friend bool operator<=(const UI &lhs, const UI &rhs);

    friend bool operator>(const UI &lhs, const UI &rhs);

    friend bool operator>=(const UI &lhs, const UI &rhs);

  protected:
    UIAttributes attr_{};
    int depth_{}; //>=0
};

#endif //SDL_GUI_UI_H
