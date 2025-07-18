#ifndef SDL_GUI_UIGROUP_H
#define SDL_GUI_UIGROUP_H

#include <set>
#include "ui.h"
#include "ui_ref.h"

class UIGroup : public UI {
  public:
    UIGroup(float x,
            float y,
            float w,
            float h,
            int depth,
            const std::set<UIRef> &ui_group);

    void Draw(float offset_x, float offset_y, float offset_zoom_rate) override;

    template<typename... Args>
    void AddObject(Args &&... args);

    ~UIGroup() override;

    std::set<UIRef> ui_group_;
};
template<typename... Args>
void UIGroup::AddObject(Args &&... args) {
  ui_group_.emplace(std::forward<Args>(args)...);
}
#endif //SDL_GUI_UIGROUP_H
