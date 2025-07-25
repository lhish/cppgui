#ifndef HOVER_CHECKER_H
#define HOVER_CHECKER_H
#include <functional>
#include <map>

#include "events.h"
#include "ui.h"
enum class MouseInteraction {
  StartHover,          // 0  MOUSE_LEFT_MOVE,MOUSE_RIGHT_MOVE,MOUSE_MOVE
  MoveHover,           // 1  MOUSE_MOVE
  LeaveAreaHover,      // specific
  StartLeftClick,      // 1  MOUSE_LEFT_DOWN
  MoveLeftClick,       // 1  MOUSE_LEFT_MOVE
  StopLeftClick,       // 1  MOUSE_LEFT_UP
  LeaveAreaLeftClick,  // specific
  StartRightClick,     // 1  MOUSE_RIGHT_DOWN
  MoveRightClick,      // 1  MOUSE_RIGHT_MOVE
  StopRightClick,      // 1  MOUSE_RIGHT_UP
  LeaveAreaRightClick  // specific
};
struct UITriggerRef {
  std::function<bool(float x, float y)> checker_;
  std::function<void(float x, float y, const MouseInteraction &)> solver_;
};

class HoverChecker {
 public:
  HoverChecker() = default;

  void Add(const std::weak_ptr<UI> &ui, const UITriggerRef &ref);

  void Solve(float x, float y, const MouseStatus &mouse_status);

  void Clear();

  void ResetDealing();

 private:
  std::map<std::weak_ptr<UI>, UITriggerRef> uis_;
  std::weak_ptr<UI> dealing_ui_;
  bool counteract_{};
  inline static const std::map<MouseStatus, MouseInteraction> mouse_map_{
      {MouseStatus::MOUSE_MOVE, MouseInteraction::MoveHover},
      {MouseStatus::MOUSE_LEFT_MOVE, MouseInteraction::MoveLeftClick},
      {MouseStatus::MOUSE_RIGHT_MOVE, MouseInteraction::MoveRightClick},
      {MouseStatus::MOUSE_LEFT_DOWN, MouseInteraction::StartLeftClick},
      {MouseStatus::MOUSE_RIGHT_DOWN, MouseInteraction::StartRightClick},
      {MouseStatus::MOUSE_LEFT_UP, MouseInteraction::StopLeftClick},
      {MouseStatus::MOUSE_RIGHT_UP, MouseInteraction::StopRightClick}};
};

#endif  // HOVER_CHECKER_H
