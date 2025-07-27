#include "sdl_gui/gui/hover_checker.h"

#include "sdl_gui/common/magic_enum.hpp"

void HoverChecker::Add(const std::weak_ptr<UI> &ui, const UITriggerRef &ref) { uis_[ui] = ref; }
void HoverChecker::Solve(std::optional<float> x, std::optional<float> y, std::optional<MouseStatus> mouse_status) {
  // todo:重构，逻辑太乱了
  if (!x || !y || !mouse_status) {
    x = last_x_;
    y = last_y_;
    mouse_status = waiting_mouse_.at(last_mouse_status_);
  } else {
    last_x_ = *x;
    last_y_ = *y;
    last_mouse_status_ = *mouse_status;
  }
  if ((mouse_status == MouseStatus::MOUSE_LEFT_UP || mouse_status == MouseStatus::MOUSE_RIGHT_UP) && counteract_) {
    counteract_ = false;
    return;
  }  // 抵消对其他ui的影响
  if (!dealing_ui_.expired() && uis_.contains(dealing_ui_)) {
    if (mouse_status == MouseStatus::MOUSE_IDLE || mouse_status == MouseStatus::MOUSE_LEFT_IDLE ||
        mouse_status == MouseStatus::MOUSE_RIGHT_IDLE) {
      ResetDealing(*x, *y, *mouse_status);
      return;
    }
    if (uis_[dealing_ui_].checker_(*x, *y)) {
      uis_[dealing_ui_].solver_(*x, *y, mouse_map_.at(*mouse_status));
      return;
    }
    ResetDealing(*x, *y, *mouse_status);
  }
  for (const auto &[ui, trigger] : uis_) {
    if (trigger.checker_(*x, *y)) {
      dealing_ui_ = ui;
      trigger.solver_(*x, *y, MouseInteraction::StartHover);
      break;
    }
  }
}

void HoverChecker::Clear() { uis_.clear(); }
void HoverChecker::ResetDealing(const float x, const float y, const MouseStatus &mouse_status) {
  if (mouse_status == MouseStatus::MOUSE_MOVE || mouse_status == MouseStatus::MOUSE_IDLE) {
    uis_[dealing_ui_].solver_(x, y, MouseInteraction::LeaveAreaHover);
  } else if (mouse_status == MouseStatus::MOUSE_LEFT_MOVE || mouse_status == MouseStatus::MOUSE_LEFT_IDLE) {
    uis_[dealing_ui_].solver_(x, y, MouseInteraction::LeaveAreaLeftClick);
    counteract_ = true;
  } else {
    uis_[dealing_ui_].solver_(x, y, MouseInteraction::LeaveAreaRightClick);
    counteract_ = true;
  }
  dealing_ui_.reset();
}
