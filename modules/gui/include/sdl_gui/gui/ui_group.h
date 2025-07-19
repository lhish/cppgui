#ifndef SDL_GUI_UIGROUP_H
#define SDL_GUI_UIGROUP_H

#include <set>
#include "ui.h"
#include "ui_ref.h"

class UIGroup : public UI {
  public:
    UIGroup(const UIAttributes &attr,
            int depth,
            const std::set<UIRef> &ui_group);

    void Draw(const UIAttributes &offset) override;

    void AddObject(const UIRef &ref) override;

    ~UIGroup() override;

    std::set<UIRef> ui_group_;
};
#endif //SDL_GUI_UIGROUP_H
