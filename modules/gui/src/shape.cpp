#include "sdl_gui/gui/shape.h"

#include "sdl_gui/gui/controller.h"

Shape::Shape(const UIAttributes &attr, std::string name, const SDL_Color &color, const float radius_ratio,
             const int depth)
    : UIGroup(attr, std::move(name), depth, {}), color_(color), radius_ratio_(radius_ratio) {}
void Shape::DrawSelf() { controller.DrawRRect(real_->x_, real_->y_, real_->w_, real_->h_, color_, radius_ratio_); }