#include "sdl_gui/gui/hover_checker.h"

#include "sdl_gui/common/magic_enum.hpp"

void HoverChecker::Add(const std::weak_ptr<UI> &ui, const UITriggerRef &ref) { uis_[ui] = ref; }

void HoverChecker::Solve(const float x, const float y, const MouseStatus &mouse_status) {
  if ((mouse_status == MouseStatus::MOUSE_LEFT_UP || mouse_status == MouseStatus::MOUSE_RIGHT_UP) && counteract_) {
    counteract_ = false;
    return;
  }  // 抵消对其他ui的影响
  if (!dealing_ui_.expired() && uis_.contains(dealing_ui_)) {
    if (mouse_status == MouseStatus::MOUSE_IDLE || mouse_status == MouseStatus::MOUSE_LEFT_IDLE ||
        mouse_status == MouseStatus::MOUSE_RIGHT_IDLE) {
      // LOG(INFO) << "HoverChecker::Solve: " << magic_enum::enum_name(MouseStatus::IDLE);
      ResetDealing(x, y, mouse_status);
      return;
    }
    if (uis_[dealing_ui_].checker_(x, y)) {
      uis_[dealing_ui_].solver_(x, y, mouse_map_.at(mouse_status));
      LOG(INFO) << "HoverChecker::Solve: " << magic_enum::enum_name(mouse_map_.at(mouse_status));
      return;
    }
    ResetDealing(x, y, mouse_status);
  }
  for (const auto &[ui, trigger] : uis_) {
    if (trigger.checker_(x, y)) {
      dealing_ui_ = ui;
      trigger.solver_(x, y, MouseInteraction::StartHover);
      break;
    }
  }
}

void HoverChecker::Clear() { uis_.clear(); }
void HoverChecker::ResetDealing(const float x, const float y, const MouseStatus &mouse_status) {
  if (mouse_status == MouseStatus::MOUSE_MOVE || mouse_status == MouseStatus::MOUSE_IDLE) {
    uis_[dealing_ui_].solver_(x, y, MouseInteraction::LeaveAreaHover);
    LOG(INFO) << "HoverChecker::Solve: " << magic_enum::enum_name(MouseInteraction::LeaveAreaHover);
  } else if (mouse_status == MouseStatus::MOUSE_LEFT_MOVE || mouse_status == MouseStatus::MOUSE_LEFT_IDLE) {
    uis_[dealing_ui_].solver_(x, y, MouseInteraction::LeaveAreaLeftClick);
    LOG(INFO) << "HoverChecker::Solve: " << magic_enum::enum_name(MouseInteraction::LeaveAreaLeftClick);
    counteract_ = true;
  } else {
    uis_[dealing_ui_].solver_(x, y, MouseInteraction::LeaveAreaRightClick);
    LOG(INFO) << "HoverChecker::Solve: " << magic_enum::enum_name(MouseInteraction::LeaveAreaRightClick);
    counteract_ = true;
  }
  dealing_ui_.reset();
}
