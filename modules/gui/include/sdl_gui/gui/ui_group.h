#ifndef SDL_GUI_UI_GROUP_H
#define SDL_GUI_UI_GROUP_H

#include <set>
#include "ui.h"

class UIGroup : public UI {
  public:
    UIGroup(const UIAttributes &attr,
            std::string name,
            int depth = 0,
            const std::set<std::shared_ptr<UI> > &ui_group = {});

    void Draw(const UIAttributes &offset) override;

    void AddObject(const std::shared_ptr<UI> &ref) override;

    ~UIGroup() override;

    std::set<std::shared_ptr<UI> > ui_group_;
};
#endif //SDL_GUI_UI_GROUP_H
