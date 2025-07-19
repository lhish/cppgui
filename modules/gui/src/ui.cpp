#include "sdl_gui/gui/ui.h"

UI::UI(const float x, const float y_ratio, const float w, const float h_ratio, const int depth)
  : x_(x),
    y_ratio_(y_ratio),
    w_(w),
    h_ratio_(h_ratio), depth_(depth) {
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
