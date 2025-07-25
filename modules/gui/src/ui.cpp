#include "sdl_gui/gui/ui.h"

#include <utility>

UI::UI(const UIAttributes &attr, std::string name, const int depth)
    : attr_(attr), depth_(depth), name_(std::move(name)) {
  counter_++;
  index_ = counter_;
}

void UI::Click() {}

void UI::AddTrigger(const std::optional<UIAttributes> &real) {}

bool operator==(const UI &lhs, const UI &rhs) { return lhs.index_ == rhs.index_; }

bool operator!=(const UI &lhs, const UI &rhs) { return !(lhs == rhs); }

bool operator<(const UI &lhs, const UI &rhs) {
  if (lhs.depth_ == rhs.depth_) {
    return lhs.index_ < rhs.index_;
  }
  return lhs.depth_ < rhs.depth_;
}

bool operator<=(const UI &lhs, const UI &rhs) { return !(rhs < lhs); }

bool operator>(const UI &lhs, const UI &rhs) { return rhs < lhs; }

bool operator>=(const UI &lhs, const UI &rhs) { return !(lhs < rhs); }
