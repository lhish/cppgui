#ifndef SDL_GUI_UI_H
#define SDL_GUI_UI_H

#include <vector>
#include <memory>

class UIRef; // Forward declaration

class UI {
  friend class UIRef;

  friend bool operator<(const UIRef &lhs, const UIRef &rhs);

  public:
    virtual ~UI() = default;

    explicit UI() = default;

    UI(float x, float y, float w, float h, int depth);

    virtual void Draw(float offset_x = 0, float offset_y = 0, float offset_zoom_rate = 1);

    virtual void Click();

    friend bool operator==(const UI &lhs, const UI &rhs);

    friend bool operator!=(const UI &lhs, const UI &rhs);

    friend bool operator<(const UI &lhs, const UI &rhs);

    friend bool operator<=(const UI &lhs, const UI &rhs);

    friend bool operator>(const UI &lhs, const UI &rhs);

    friend bool operator>=(const UI &lhs, const UI &rhs);

  protected:
    float x_{}, y_{}; //0-1
    float w_{}, h_{}; //0-1
    int depth_{}; //>=0
    float zoom_rate_{1};
};

#endif //SDL_GUI_UI_H