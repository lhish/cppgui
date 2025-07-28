#ifndef SDL_GUI_BUTTON_H
#define SDL_GUI_BUTTON_H

#include <SDL3/SDL.h>

#include "shape.h"

class Button : public Shape {
 public:
  Button(const UIAttributes &attr, std::string name, const SDL_Color &color, float radius_ratio, int depth = 0);
  void DrawKids() override;
  void DrawSelf() override;

  void AddTrigger() override;

 private:
  float height_{1};
  std::weak_ptr<UI> circle_;
};

UI_FACTORY_REGISTER(UIType::Button, Button)
#endif  // SDL_GUI_BUTTON_H
