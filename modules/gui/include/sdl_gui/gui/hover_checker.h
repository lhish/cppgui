#ifndef HOVER_CHECKER_H
#define HOVER_CHECKER_H
#include <functional>
#include <set>

#include "events.h"
#include "ui.h"

struct UITriggerRef {
  std::shared_ptr<UI> ref_;
  std::function<bool(float x, float y)> checker_; //real
  std::function<void(float x, float y, MouseStatus &)> solver_;

  friend bool operator==(const UITriggerRef &lhs, const UITriggerRef &rhs) {
    return *lhs.ref_ == *rhs.ref_;
  }

  friend bool operator!=(const UITriggerRef &lhs, const UITriggerRef &rhs) {
    return !(lhs == rhs);
  }

  friend bool operator<(const UITriggerRef &lhs, const UITriggerRef &rhs) {
    return *lhs.ref_ < *rhs.ref_;
  }

  friend bool operator<=(const UITriggerRef &lhs, const UITriggerRef &rhs) {
    return !(rhs < lhs);
  }

  friend bool operator>(const UITriggerRef &lhs, const UITriggerRef &rhs) {
    return rhs < lhs;
  }

  friend bool operator>=(const UITriggerRef &lhs, const UITriggerRef &rhs) {
    return !(lhs < rhs);
  }
};

class HoverChecker {
  public:
    HoverChecker() = default;

    void Add(const UITriggerRef &ref);

    void Solve(float x, float y, MouseStatus &mouse_status) const;

    void Clear();

  private:
    std::set<UITriggerRef> uis_;
};


#endif //HOVER_CHECKER_H
