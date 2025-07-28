#ifndef SDL_GUI_SHAPE_H
#define SDL_GUI_SHAPE_H

#include <SDL3/SDL.h>

#include "ui_group.h"

class Shape : public UIGroup {
 public:
  Shape(const UIAttributes &attr, std::string name, const SDL_Color &color, float radius_ratio, int depth = 0);

  void DrawSelf() override;

 protected:
  SDL_Color color_{};
  float radius_ratio_{};
};

UI_FACTORY_REGISTER(UIType::Shape, Shape)
#endif  // SDL_GUI_SHAPE_H
