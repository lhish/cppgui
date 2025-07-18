#include "../include/sdl_gui/ui_group.h"

UIGroup::UIGroup(const float x,
                 const float y,
                 const float w,
                 const float h,
                 const int depth,
                 const std::set<UIRef> &ui_group) : UI(x, y, w, h, depth),
                                                    ui_group_(ui_group) {
}

void UIGroup::Draw(const float offset_x, const float offset_y, const float offset_zoom_rate) {
  for (auto &ui : ui_group_) {
    ui->Draw(x_ + offset_x, y_ + offset_y, offset_zoom_rate * zoom_rate_);
  }
}

UIGroup::~UIGroup() = default;