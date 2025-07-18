#ifndef SDL_GUI_BUTTON_H
#define SDL_GUI_BUTTON_H

#include <SDL3/SDL.h>
#include "ui.h"

class Button : public UI {
  public:
    Button(float x,
           float y,
           float w,
           float h,
           int depth,
           const SDL_Color &color,
           float radius_ratio);

    void Draw(float offset_x, float offset_y, float offset_zoom_rate) override;

  private:
    SDL_Color color_{255, 0, 0, 255};
    float radius_ratio_{};
};

#endif //SDL_GUI_BUTTON_H