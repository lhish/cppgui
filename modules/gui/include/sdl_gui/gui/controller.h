#ifndef SDL_GUI_CONTROLLER_H
#define SDL_GUI_CONTROLLER_H

#include <SDL3/SDL.h>
#include <memory>

#include <include/core/SkCanvas.h>
#include <include/core/SkColor.h>
#include <include/core/SkSurface.h>
#include <include/core/SkPaint.h>

#include "events.h"
#include "hover_checker.h"
#include "ui.h"


class Controller {
  public:
    static Controller &GetInstance();

    void handle_events();

    void Draw() const;

    void StartLoop(
#ifdef DEBUG
      const std::optional<std::function<void()> > &debug_draw = {}
#endif
    );

    ~Controller();

    [[nodiscard]] std::optional<UIAttributes> CalReal(const UIAttributes &offset, const UIAttributes &self) const;

    [[nodiscard]] bool CheckRange(float x, float y) const;

    void AddTrigger(const UITriggerRef &trigger_ref);

    std::shared_ptr<UI> AddObject(const std::shared_ptr<UI> &ui,
                                  const std::optional<std::shared_ptr<UI> > &parent = {});

    static SDL_Color SkColorToSDLColor(const SkColor &color);

    void SetColor(const SDL_Color &color);

    void DrawRect(float x, float y, float w, float h, const SDL_Color &color);

    void DrawRRect(float x, float y, float w, float h, const SDL_Color &color, float radius_ratio);

    void DrawRRectShadow(float x,
                         float y,
                         float w,
                         float h,
                         const SDL_Color &color,
                         float radius,
                         float elevation);

    [[nodiscard]] int GetWidth() const { return width_; }

    [[nodiscard]] int GetHeight() const { return height_; }

  private:
    Controller();

    uint32_t windowFlags{};
    SDL_GLContext glContext_{};
    SkCanvas *canvas_;
    SDL_Event event_{};
    SDL_Window *window_;
    std::optional<std::shared_ptr<UI> > basic_ui_{};
    bool keep_going{true};
    int width_;
    int ori_width_;
    int height_;
    int ori_height_;
    SDL_DisplayID *displays_;
    const SDL_DisplayMode *dm_;
    SkPaint paint_{};
    std::unique_ptr<sk_sp<SkSurface> > surface_;
    HoverChecker hover_checker_;
    MouseStatus mouse_down_{MouseStatus::IDLE};
};

inline auto &controller = Controller::GetInstance();
#endif //SDL_GUI_CONTROLLER_H
