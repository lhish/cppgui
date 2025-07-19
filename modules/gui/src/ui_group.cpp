#include "sdl_gui/gui/ui_group.h"

#include <src/base/SkRandom.h>

#include "sdl_gui/gui/controller.h"

UIGroup::UIGroup(const UIAttributes &attr,
                 const int depth,
                 const std::set<UIRef> &ui_group) : UI(attr, depth),
                                                    ui_group_(ui_group) {
}

void UIGroup::Draw(const UIAttributes &offset) {
  const auto real = Controller::CalReal(offset, attr_);
#ifdef DEBUG
  static SkRandom rand;
  static auto color_here = rand.nextU() | 0x44808080;
  controller.DrawRect(real.x_, real.y_, real.w_, real.h_, Controller::SkColorToSDLColor(color_here));
#endif
  for (auto &ui : ui_group_) {
    ui->Draw(real);
  }
}

void UIGroup::AddObject(const UIRef &ref) {
  ui_group_.emplace(ref);
}

UIGroup::~UIGroup() = default;
