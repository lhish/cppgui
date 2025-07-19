#ifndef SDL_GUI_UI_H
#define SDL_GUI_UI_H

class UIRef;

class UI {
  friend class UIRef;

  friend bool operator<(const UIRef &lhs, const UIRef &rhs);

  public:
    virtual ~UI() = default;

    explicit UI() = default;

    UI(float x, float y_ratio, float w, float h_ratio, int depth);

    virtual void Draw(float offset_x = 0, float offset_y = 9.0 / 16, float offset_zoom_rate = 1) {
    };

    virtual void Click();

    friend bool operator==(const UI &lhs, const UI &rhs);

    friend bool operator!=(const UI &lhs, const UI &rhs);

    friend bool operator<(const UI &lhs, const UI &rhs);

    friend bool operator<=(const UI &lhs, const UI &rhs);

    friend bool operator>(const UI &lhs, const UI &rhs);

    friend bool operator>=(const UI &lhs, const UI &rhs);

  protected:
    float x_{}; //0-1
    float y_ratio_{}; //>0
    float w_{}; //0-1
    float h_ratio_{}; //>0
    int depth_{}; //>=0
    float zoom_rate_{1};
};

#endif //SDL_GUI_UI_H
