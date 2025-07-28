#ifndef SDL_GUI_UI_GROUP_H
#define SDL_GUI_UI_GROUP_H

#include <set>

#include "ui.h"

class UIGroup : public UI {
 public:
  UIGroup(const UIAttributes &attr, std::string name, int depth = 0,
          const std::set<std::shared_ptr<UI> > &ui_group = {});

  void DrawReal() final;
  virtual void DrawSelf();
  virtual void DrawKids();

  void AddObject(const std::shared_ptr<UI> &ref) override;

  ~UIGroup() override;

  std::set<std::shared_ptr<UI> > ui_group_;
};

UI_FACTORY_REGISTER(UIType::UIGroup, UIGroup)
#endif  // SDL_GUI_UI_GROUP_H
