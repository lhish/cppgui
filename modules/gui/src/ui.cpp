#include "sdl_gui/gui/ui.h"

#include <utility>

#include "sdl_gui/gui/controller.h"

UI::UI(const UIAttributes &attr, std::string name, const int depth)
    : attr_(attr), depth_(depth), name_(std::move(name)) {
  counter_++;
  index_ = counter_;
}
void UI::Draw(const UIAttributes &offset) {
  if (CalReal(offset)) {
    DrawReal();
    AddTrigger();
  }
}
bool UI::CalReal(const UIAttributes &offset) {
  real_ = controller.CalReal(offset, attr_);
  if (!real_) {
    return false;
  }
  return true;
}

void UI::Click() {}

void UI::AddTrigger() {}
const UIAttributes &UI::get_real() {
  assertm(real_, "real_ is nullptr");
  return *real_;
}

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
