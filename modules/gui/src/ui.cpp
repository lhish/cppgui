#include "sdl_gui/gui/ui.h"

#include <utility>

#include "sdl_gui/gui/controller.h"

float UI::FromBottomY(float y) { return y + from_bottom_; }
UI::UI(const UIAttributes &attr, std::string name, const int depth)
    : attr_(attr), depth_(depth), name_(std::move(name)) {
  counter_++;
  index_ = counter_;
}
void UI::Draw(const UIAttributes &offset) {
  if (visibility_ && CalReal(offset)) {
    DrawReal();
    AddTrigger();
  }
}
bool UI::CalReal(const UIAttributes &offset) {
  if (const auto &real = controller.CalReal(offset, attr_); real) {
    real_ = *real;
    return true;
  }
  return false;
}

void UI::Click() {}

void UI::AddTrigger() {}
const UIAttributes &UI::get_real() const { return real_; }
void UI::set_visibility(const bool visibility) { visibility_ = visibility; }

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
