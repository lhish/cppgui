#include "sdl_gui/gui/ui.h"

UI::UI(UIAttributes attr, int depth): attr_(attr), depth_(depth) {
}

void UI::Click() {
}

bool operator==(const UI &lhs, const UI &rhs) {
  return lhs.depth_ == rhs.depth_;
}

bool operator!=(const UI &lhs, const UI &rhs) {
  return !(lhs == rhs);
}

bool operator<(const UI &lhs, const UI &rhs) {
  return lhs.depth_ < rhs.depth_;
}

bool operator<=(const UI &lhs, const UI &rhs) {
  return !(rhs < lhs);
}

bool operator>(const UI &lhs, const UI &rhs) {
  return rhs < lhs;
}

bool operator>=(const UI &lhs, const UI &rhs) {
  return !(lhs < rhs);
}
