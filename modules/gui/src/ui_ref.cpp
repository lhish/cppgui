#include "sdl_gui/gui/ui_ref.h"

UIRef::UIRef(std::vector<std::unique_ptr<UI> > &base_ui_vec,
             const std::vector<std::unique_ptr<UI> >::iterator it) : base_ui_vec_(base_ui_vec),
                                                                     index_(it - base_ui_vec.begin()) {
}

UI &UIRef::operator*() const {
  return *base_ui_vec_.get()[index_];
}

UI *UIRef::operator->() const {
  return base_ui_vec_.get()[index_].get();
}

UIRef::operator UI &() const {
  return *base_ui_vec_.get()[index_];
}

UIRef::operator bool() const {
  return index_ < base_ui_vec_.get().size();
}

bool operator==(const UIRef &lhs, const UIRef &rhs) {
  return lhs.index_ == rhs.index_;
}

bool operator!=(const UIRef &lhs, const UIRef &rhs) {
  return !(lhs == rhs);
}

bool operator<(const UIRef &lhs, const UIRef &rhs) {
  if (lhs.base_ui_vec_.get()[lhs.index_]->depth_ != rhs.base_ui_vec_.get()[rhs.index_]->depth_) {
    return lhs.base_ui_vec_.get()[lhs.index_]->depth_ < rhs.base_ui_vec_.get()[rhs.index_]->depth_;
  }
  return lhs.index_ < rhs.index_;
}

bool operator<=(const UIRef &lhs, const UIRef &rhs) {
  return !(rhs < lhs);
}

bool operator>(const UIRef &lhs, const UIRef &rhs) {
  return rhs < lhs;
}

bool operator>=(const UIRef &lhs, const UIRef &rhs) {
  return !(lhs < rhs);
}
