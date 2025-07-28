#ifndef SDL_GUI_CONTROLLER_H
#define SDL_GUI_CONTROLLER_H

#include <SDL3/SDL.h>
#include <include/core/SkCanvas.h>
#include <include/core/SkColor.h>
#include <include/core/SkPaint.h>
#include <include/core/SkSurface.h>

#include <map>
#include <memory>
#include <set>

#include "animation.h"
#include "button.h"
#include "events.h"
#include "hover_checker.h"
#include "ui.h"
#include "ui_group.h"

class Controller {
 public:
  static Controller &GetInstance();

  bool handle_events();

  void Draw() const;

  void StartLoop(
#ifdef DEBUG
      const std::optional<std::function<void()>> &debug_draw = {}
#endif
  );

  ~Controller();

  [[nodiscard]] std::optional<UIAttributes> CalReal(const UIAttributes &offset, const UIAttributes &self) const;

  [[nodiscard]] bool CheckRange(float x, float y) const;

  void AddTrigger(const std::weak_ptr<UI> &ui, const UITriggerRef &trigger_ref);

  void AddAnimation(Animation &&animation);

  std::shared_ptr<UI> AddObject(const std::shared_ptr<UI> &ui,
                                const std::optional<std::shared_ptr<UI>> &parent = {}) const;
  bool ExistAnimation(const std::weak_ptr<UI> &ui) const;

  static SDL_Color SkColorToSDLColor(const SkColor &color);

  void SetColor(const SDL_Color &color);

  void DrawRRect(float x, float y, float w, float h, const SDL_Color &color, float radius_ratio);

  void AddClipRRect(float x, float y, float w, float h, float radius_ratio) const;

  void DrawRRectShadow(float x, float y, float w, float h, const SDL_Color &color, float radius, float elevation);

  void SaveCanvas() const;

  void RestoreCanvas() const;

  [[nodiscard]] int GetWidth() const { return width_; }

  [[nodiscard]] int GetHeight() const { return height_; }

  void UpdateAnimation();

  bool SolveSingleEvent(const SDL_Event &event);

  [[nodiscard]] std::pair<float, float> MousePositionChange(const std::weak_ptr<UI> &ui, float x, float y) const;

  template <typename T>
  void Store(T &ref);

  template <typename T>
  T WithDraw(T &ref);

  template <typename T>
  void Reset(T &ref);

 private:
  Controller();

  uint32_t windowFlags{};
  SDL_GLContext glContext_{};
  SkCanvas *canvas_;
  SDL_Event event_{};
  SDL_Window *window_;
  std::optional<std::shared_ptr<UI>> basic_ui_{};
  bool keep_going{true};
  int width_;
  int ori_width_;
  int height_;
  int ori_height_;
  SDL_DisplayID *displays_;
  const SDL_DisplayMode *dm_;
  SkPaint paint_{};
  std::unique_ptr<sk_sp<SkSurface>> surface_;
  HoverChecker hover_checker_;
  std::map<std::weak_ptr<UI>, std::set<Animation>> animations_;
  bool initialized_{};
  std::map<std::shared_ptr<FloatNumberRef>, std::any> origin_;
};
template <typename T>
void Controller::Store(T &ref) {
  const auto ref_shared = std::make_shared<FloatNumberRefT<T>>(ref);
  if (origin_.contains(ref_shared)) {
    return;
  }
  origin_[ref_shared].template emplace<T>(ref);
}
template <typename T>
T Controller::WithDraw(T &ref) {
  const auto ref_shared = std::make_shared<FloatNumberRefT<T>>(ref);
  return std::any_cast<T>(origin_[ref_shared]);
}
template <typename T>
void Controller::Reset(T &ref) {
  const auto ref_shared = std::make_shared<FloatNumberRefT<T>>(ref);
  if (origin_.contains(ref_shared)) {
    return;
  }
  origin_.erase(ref_shared);
}

inline auto &controller = Controller::GetInstance();
#endif  // SDL_GUI_CONTROLLER_H
