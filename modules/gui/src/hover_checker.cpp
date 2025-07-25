#include "sdl_gui/gui/hover_checker.h"

void HoverChecker::Add(const std::weak_ptr<UI> &ui, const UITriggerRef &ref) { uis_[ui] = ref; }

void HoverChecker::Solve(const float x, const float y, const MouseStatus &mouse_status) {
  if ((mouse_status == MouseStatus::MOUSE_LEFT_UP || mouse_status == MouseStatus::MOUSE_RIGHT_UP) && counteract_) {
    counteract_ = false;
    return;
  }
  if (!dealing_ui_.expired() && uis_.contains(dealing_ui_)) {
    if (uis_[dealing_ui_].checker_(x, y)) {
      uis_[dealing_ui_].solver_(x, y, mouse_map_.at(mouse_status));
      return;
    }
    if (mouse_status == MouseStatus::MOUSE_MOVE) {
      uis_[dealing_ui_].solver_(x, y, MouseInteraction::LeaveAreaHover);
    } else if (mouse_status == MouseStatus::MOUSE_LEFT_MOVE) {
      uis_[dealing_ui_].solver_(x, y, MouseInteraction::LeaveAreaLeftClick);
      counteract_ = true;
    } else {
      uis_[dealing_ui_].solver_(x, y, MouseInteraction::LeaveAreaRightClick);
      counteract_ = true;
    }
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
void HoverChecker::ResetDealing() { dealing_ui_.reset(); }
