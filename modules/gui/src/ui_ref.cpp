#include "../include/sdl_gui/ui_ref.h"

UIRef::UIRef(std::vector<std::unique_ptr<UI> > &base_ui_vec,
             const std::vector<std::unique_ptr<UI> >::iterator it) : base_ui_vec_(base_ui_vec),
                                                                     index_(it - base_ui_vec.begin()) {
}

UI &UIRef::operator*() const {
  return *base_ui_vec_[index_];
}

UI *UIRef::operator->() const {
  return base_ui_vec_[index_].get();
}

UIRef::operator UI &() const {
  return *base_ui_vec_[index_];
}

UIRef::operator bool() const {
  return index_ < base_ui_vec_.size();
}

bool operator==(const UIRef &lhs, const UIRef &rhs) {
  return lhs.index_ == rhs.index_;
}

bool operator!=(const UIRef &lhs, const UIRef &rhs) {
  return !(lhs == rhs);
}

bool operator<(const UIRef &lhs, const UIRef &rhs) {
  if (lhs.base_ui_vec_[lhs.index_]->depth_ != rhs.base_ui_vec_[rhs.index_]->depth_) {
    return lhs.base_ui_vec_[lhs.index_]->depth_ < rhs.base_ui_vec_[rhs.index_]->depth_;
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