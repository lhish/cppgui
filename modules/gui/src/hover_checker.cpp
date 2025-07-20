#include "sdl_gui/gui/hover_checker.h"
#include"glog/logging.h"
#include "sdl_gui/gui/ui.h"

void HoverChecker::Add(const UITriggerRef &ref) {
  uis_.emplace(ref);
}

void HoverChecker::Solve(const float x, const float y, MouseStatus &mouse_status) const {
  for (auto &ui : uis_) {
    // if (mouse_status == MouseStatus::MOUSE_LEFT_DOWN) {
    // DLOG(INFO) << "checking:" << ui.ref_->getname();
    // }
    if (ui.checker_(x, y)) {
      ui.solver_(x, y, mouse_status);
      break;
    }
  }
}

void HoverChecker::Clear() {
  uis_.clear();
}
