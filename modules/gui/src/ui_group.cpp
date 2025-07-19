#include "sdl_gui/gui/ui_group.h"

#include <src/base/SkRandom.h>

#include "sdl_gui/gui/controller.h"

UIGroup::UIGroup(const float x,
                 const float y_ratio,
                 const float w,
                 const float h_ratio,
                 const int depth,
                 const std::set<UIRef> &ui_group) : UI(x, y_ratio, w, h_ratio, depth),
                                                    ui_group_(ui_group) {
}

void UIGroup::Draw(const float offset_x, const float offset_y, const float offset_zoom_rate) {
  const float final_zoom_rate = offset_zoom_rate * zoom_rate_;
#ifdef DEBUG
  static SkRandom rand;
  static auto color_here = rand.nextU() | 0x44808080;
  controller.DrawRect(x_ * final_zoom_rate + offset_x,
                      x_ * y_ratio_ * final_zoom_rate + offset_y,
                      final_zoom_rate * w_,
                      final_zoom_rate * w_ * h_ratio_,
                      Controller::SkColorToSDLColor(color_here));
#endif
  for (auto &ui : ui_group_) {
    ui->Draw(x_ * final_zoom_rate + offset_x, x_ * y_ratio_ * final_zoom_rate + offset_y, final_zoom_rate);
  }
}

UIGroup::~UIGroup() = default;
