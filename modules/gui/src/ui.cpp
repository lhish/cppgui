#include "../include/sdl_gui/ui.h"

UI::UI(const float x, const float y, const float w, const float h, const int depth)
  : x_(x),
    y_(y),
    w_(w),
    h_(h), depth_(depth) {
}

void UI::Draw(float offset_x, float offset_y, float offset_zoom_rate) {
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