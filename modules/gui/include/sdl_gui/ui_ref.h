#ifndef SDL_GUI_UI_REF_H
#define SDL_GUI_UI_REF_H

#include <vector>
#include <memory>
#include "ui.h"

class UIRef {
  public:
    UIRef(std::vector<std::unique_ptr<UI> > &base_ui_vec,
          std::vector<std::unique_ptr<UI> >::iterator it);

    UI &operator*() const;

    UI *operator->() const;

    explicit operator UI &() const;

    explicit operator bool() const;

    friend bool operator==(const UIRef &lhs, const UIRef &rhs);

    friend bool operator!=(const UIRef &lhs, const UIRef &rhs);

    friend bool operator<(const UIRef &lhs, const UIRef &rhs);

    friend bool operator<=(const UIRef &lhs, const UIRef &rhs);

    friend bool operator>(const UIRef &lhs, const UIRef &rhs);

    friend bool operator>=(const UIRef &lhs, const UIRef &rhs);

  private:
    std::vector<std::unique_ptr<UI> > &base_ui_vec_;
    size_t index_;
};

#endif //SDL_GUI_UI_REF_H
