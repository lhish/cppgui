#ifndef SDL_GUI_UI_REF_H
#define SDL_GUI_UI_REF_H

#include <vector>
#include <memory>
#include "ui.h"

class UIRef {
  public:
    UIRef(std::vector<std::unique_ptr<UI> > &base_ui_vec,
          std::vector<std::unique_ptr<UI> >::iterator it);

    UIRef(const UIRef &other) = default;

    UIRef(UIRef &&other) noexcept
      : base_ui_vec_(other.base_ui_vec_),
        index_(other.index_) {
    }

    UIRef &operator=(const UIRef &other) {
      if (this == &other)
        return *this;
      base_ui_vec_ = other.base_ui_vec_;
      index_ = other.index_;
      return *this;
    }

    UIRef &operator=(UIRef &&other) noexcept {
      if (this == &other)
        return *this;
      base_ui_vec_ = other.base_ui_vec_;
      index_ = other.index_;
      return *this;
    }

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
    std::reference_wrapper<std::vector<std::unique_ptr<UI> > > base_ui_vec_;
    size_t index_;
};

#endif //SDL_GUI_UI_REF_H
