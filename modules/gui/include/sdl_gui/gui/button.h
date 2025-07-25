#ifndef SDL_GUI_BUTTON_H
#define SDL_GUI_BUTTON_H

#include <SDL3/SDL.h>

#include "ui_group.h"

class Button : public UIGroup {
 public:
  Button(const UIAttributes &attr, std::string name, const SDL_Color &color, float radius_ratio, int depth = 0);

  void Draw(const UIAttributes &offset) override;
  void AddTrigger(const std::optional<UIAttributes> &real) override;

 private:
  SDL_Color color_{255, 0, 0, 255};
  float radius_ratio_{};
  float height_{};
};

UI_FACTORY_REGISTER(UIType::Button, Button)
#endif  // SDL_GUI_BUTTON_H
