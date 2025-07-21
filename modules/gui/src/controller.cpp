#include "sdl_gui/gui/controller.h"
#include "sdl_gui/gui/controller.h"
#include "sdl_gui/gui/controller.h"

#include <cassert>
#include <ranges>
#include <variant>
#include "sdl_gui/common/sdl_utils.h"
#include <include/gpu/ganesh/GrBackendSurface.h>
#include <include/gpu/ganesh/GrDirectContext.h>
#include <include/gpu/ganesh/GrRecordingContext.h>
#include <include/gpu/ganesh/SkSurfaceGanesh.h>
#include <include/gpu/ganesh/gl/GrGLBackendSurface.h>
#include <include/gpu/ganesh/gl/GrGLDirectContext.h>
#include <include/gpu/ganesh/gl/GrGLInterface.h>
#include <src/gpu/ganesh/gl/GrGLUtil.h>
#include <include/core/SkColorSpace.h>
#include <include/core/SkRRect.h>
#include <include/effects/SkBlurMaskFilter.h>

#include "SDL3/SDL_opengl.h"
#include "sdl_gui/common/magic_enum.hpp"
#include "sdl_gui/gui/ui_group.h"

Controller &Controller::GetInstance() {
  static Controller controller;
  return controller;
}

void Controller::handle_events() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
      case SDL_EVENT_WINDOW_RESIZED:
        if (event.window.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED ||
            event.window.type == SDL_EVENT_WINDOW_RESIZED) {
          width_ = event.window.data1;
          height_ = event.window.data2;
          LOG(INFO) << "Window resize: " << width_ << " " << height_ << std::endl;
        }
        break;
      case SDL_EVENT_KEY_DOWN: {
        SDL_Keycode key = event.key.key;
        if (key == SDLK_ESCAPE) {
          keep_going = false;
        }
        break;
      }
      case SDL_EVENT_MOUSE_BUTTON_DOWN: {
        mouse_down_ = event.button.button == SDL_BUTTON_LEFT
                        ? MouseStatus::MOUSE_LEFT_DOWN
                        : MouseStatus::MOUSE_RIGHT_DOWN;
        break;
      }
      case SDL_EVENT_MOUSE_BUTTON_UP: {
        mouse_down_ = mouse_down_ == MouseStatus::MOUSE_LEFT_DOWN || mouse_down_ == MouseStatus::MOUSE_LEFT_MOVE
                        ? MouseStatus::MOUSE_LEFT_UP
                        : MouseStatus::MOUSE_RIGHT_UP;
        break;
      }
      case SDL_EVENT_MOUSE_MOTION: {
        if (mouse_down_ == MouseStatus::MOUSE_LEFT_DOWN || mouse_down_ == MouseStatus::MOUSE_LEFT_MOVE) {
          mouse_down_ = MouseStatus::MOUSE_LEFT_MOVE;
        } else if (mouse_down_ == MouseStatus::MOUSE_RIGHT_DOWN || mouse_down_ == MouseStatus::MOUSE_RIGHT_MOVE) {
          mouse_down_ = MouseStatus::MOUSE_RIGHT_MOVE;
        } else {
          mouse_down_ = MouseStatus::MOUSE_MOVE;
        }
        break;
      }
      case SDL_EVENT_QUIT:
        keep_going = false;
        break;
      default:
        if (mouse_down_ == MouseStatus::MOUSE_MOVE) {
          mouse_down_ = MouseStatus::IDLE;
        }
        break;
    }
    if (mouse_down_ != MouseStatus::IDLE) {
      hover_checker_.Solve(event.button.x, event.button.y, mouse_down_);
    }
  }
}

void Controller::Draw() const {
  canvas_->clear(SK_ColorWHITE);
  canvas_->translate(0, static_cast<float>(ori_height_ - height_));
  (*basic_ui_)->Draw({0, 0, 1, static_cast<float>(height_) / static_cast<float>(width_), 1});
  canvas_->translate(0, static_cast<float>(-(ori_height_ - height_)));
}

void Controller::StartLoop(
#ifdef DEBUG
  const std::optional<std::function<void()> > &debug_draw
#endif
) {
  FPSTimer timer;
  while (keep_going) {
    // timer.Tick();
    handle_events();
    hover_checker_.Clear();
#ifdef DEBUG
    if (debug_draw) {
      canvas_->clear(SK_ColorWHITE);
      canvas_->translate(0, static_cast<float>(ori_height_ - height_));
      (*debug_draw)();
      canvas_->translate(0, static_cast<float>(-(ori_height_ - height_)));
    } else {
#endif
      Draw();
#ifdef DEBUG
    }
#endif
    if (auto dContext = GrAsDirectContext(canvas_->recordingContext())) {
      dContext->flushAndSubmit();
    }
    SDL_GL_SwapWindow(window_);
  }
}

Controller::~Controller() {
  SDL_GL_DestroyContext(glContext_);
  SDL_DestroyWindow(window_);
  SDL_Quit();
}

std::optional<UIAttributes> Controller::CalReal(const UIAttributes &offset, const UIAttributes &self) const {
  const auto final_zoom_rate = offset.zoom_rate_ * self.zoom_rate_;
  const auto x_real = self.x_ * final_zoom_rate + offset.x_;
  const auto y_real = self.y_ >= 0
                        ? self.y_ * final_zoom_rate + offset.y_
                        : offset.h_ + self.y_ * final_zoom_rate + offset.y_; //<0代表以左下为原点
  const auto w_real = self.w_ * final_zoom_rate;
  const auto h_real = self.h_ != -1 //-1代表一直延伸到底部
                        ? self.w_ * self.h_ * final_zoom_rate
                        : self.y_ >= 0
                            ? offset.h_ - self.y_ * final_zoom_rate
                            : -self.y_ * final_zoom_rate;
  if (CheckRange(x_real, y_real) && CheckRange(x_real, y_real + h_real) && CheckRange(x_real + w_real, h_real) &&
      CheckRange(x_real + w_real, y_real + h_real)) {
    return {};
  }
  return {
    {
      x_real, y_real, w_real,
      h_real, final_zoom_rate
    }
  };
}

bool Controller::CheckRange(float x, float y) const {
  return x < 0 || x > 1 || y < 0 || y > static_cast<float>(height_) / static_cast<float>(width_);
}

void Controller::AddTrigger(const UITriggerRef &trigger_ref) {
  hover_checker_.Add(trigger_ref);
}

UIRef Controller::AddObject(std::unique_ptr<UI> ui, const std::optional<std::reference_wrapper<UIRef> > parent) {
  ui->depth_ += (parent ? parent->get() : *basic_ui_)->depth_ + 1;
  ui_group_.emplace_back(std::move(ui));
  UIRef ref{ui_group_, ui_group_.end() - 1};
  ui_group_.back()->SetUIRef(ref);
  if (!parent) {
    (*basic_ui_)->AddObject(ref);
  } else {
    parent->get()->AddObject(ref);
  }
  return ref;
}

SDL_Color Controller::SkColorToSDLColor(const SkColor &color) {
  return SDL_Color(SkColorGetR(color),SkColorGetG(color),SkColorGetB(color),SkColorGetA(color));
}

void Controller::SetColor(const SDL_Color &color) {
  paint_.setColor(SkColorSetARGB(color.a, color.r, color.g, color.b));
}


void Controller::DrawRect(const float x,
                          const float y,
                          const float w,
                          const float h,
                          const SDL_Color &color) {
  SetColor(color);
  canvas_->drawRect(SkRect::MakeXYWH(SkIntToScalar(x*width_),
                                     SkIntToScalar(y*width_),
                                     SkIntToScalar(w*width_),
                                     SkIntToScalar(h*width_)),
                    paint_);
}

void Controller::DrawRRectShadow(const float x,
                                 const float y,
                                 const float w,
                                 const float h,
                                 const SDL_Color &color,
                                 const float radius,
                                 ShadowHeights elevation) {
  const float real_x = x * static_cast<float>(width_);
  const float real_y = y * static_cast<float>(width_);
  const float real_w = w * static_cast<float>(width_);
  const float real_h = h * static_cast<float>(width_);
  const float corner_radius = std::min(w, h) * radius * static_cast<float>(width_) / 2;
  const float ratio = static_cast<float>(width_) / 1280;
  for (auto [offset_y, radius_ratio, extend, alpha] :
       std::ranges::reverse_view(shadow_properties_map[elevation].layer)) {
    paint_.setAntiAlias(true);
    paint_.setColor(SkColorSetARGB(static_cast<int>(alpha * 255), 0, 0, 0));
    paint_.setMaskFilter(SkMaskFilter::MakeBlur(kNormal_SkBlurStyle, radius_ratio * ratio / 1.3f, true));
    canvas_->save();
    canvas_->clipRRect(
      SkRRect::MakeRectXY(SkRect::MakeXYWH(real_x, real_y, real_w, real_h), corner_radius, corner_radius),
      SkClipOp::kDifference,
      true);
    canvas_->drawRRect(SkRRect::MakeRectXY(
                         SkRect::MakeXYWH(real_x - extend * ratio,
                                          real_y - extend * ratio + offset_y * ratio * 1.5f,
                                          real_w + extend * ratio * 2,
                                          real_h + extend * ratio * 2),
                         corner_radius,
                         corner_radius),
                       paint_);
    canvas_->restore();
    paint_.reset();
  }
}

void Controller::DrawRRect(const float x,
                           const float y,
                           const float w,
                           const float h,
                           const SDL_Color &color,
                           const float radius_ratio) {
  SetColor(color);
  const float corner_radius = std::min(w, h) * radius_ratio * static_cast<float>(width_) / 2;
  canvas_->drawRRect(SkRRect::MakeRectXY(SkRect::MakeXYWH(SkIntToScalar(x*width_),
                                                          SkIntToScalar(y*width_),
                                                          SkIntToScalar(w*width_),
                                                          SkIntToScalar(h*width_)),
                                         corner_radius,
                                         corner_radius),
                     paint_);
}

Controller::Controller() {
  SDL_SetLogPriorities(SDL_LOG_PRIORITY_VERBOSE);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
  static constexpr int kStencilBits = 8;
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, kStencilBits);

  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

  static constexpr int kMsaaSampleCount = 4;
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, kMsaaSampleCount);

  CheckSDL(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS));

  int num_displays = 0;
  displays_ = CheckSDL(SDL_GetDisplays(&num_displays));
  if (num_displays < 1) {
    handle_error();
  }
  dm_ =
      CheckSDL(SDL_GetDesktopDisplayMode(displays_[0]));
  width_ = dm_->w;
  height_ = dm_->h;
  ori_width_ = dm_->w;
  ori_height_ = dm_->h;

  window_ =
      CheckSDL(SDL_CreateWindow("Hello, SDL3!", dm_->w, dm_->h, windowFlags));

  glContext_ = CheckSDL(SDL_GL_CreateContext(window_));
  CheckSDL(SDL_GL_MakeCurrent(window_, glContext_));
  uint32_t windowFormat = SDL_GetWindowPixelFormat(window_);
  glViewport(0, 0, dm_->w, dm_->h);
  glClearColor(1, 1, 1, 1);
  glClearStencil(0);
  glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // setup GrContext
  auto interface = GrGLMakeNativeInterface();

  sk_sp<GrDirectContext> grContext(GrDirectContexts::MakeGL(interface));
  SkASSERT(grContext);

  GrGLint buffer;
  GR_GL_GetIntegerv(interface.get(), GR_GL_FRAMEBUFFER_BINDING, &buffer);
  GrGLFramebufferInfo info;
  info.fFBOID = static_cast<GrGLuint>(buffer);
  int contextType;
  CheckSDL(SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &contextType));
  SkColorType colorType;
  if (SDL_PIXELFORMAT_RGBA8888 == windowFormat) {
    info.fFormat = GR_GL_RGBA8;
    colorType = kRGBA_8888_SkColorType;
  } else {
    colorType = kBGRA_8888_SkColorType;
    if (SDL_GL_CONTEXT_PROFILE_ES == contextType) {
      info.fFormat = GR_GL_BGRA8;
    } else {
      info.fFormat = GR_GL_RGBA8;
    }
  }
  GrBackendRenderTarget target = GrBackendRenderTargets::MakeGL(width_,
                                                                height_,
                                                                kMsaaSampleCount,
                                                                kStencilBits,
                                                                info);

  SkSurfaceProps props;

  surface_ = std::make_unique<sk_sp<SkSurface> >(SkSurfaces::WrapBackendRenderTarget(grContext.get(),
    target,
    kBottomLeft_GrSurfaceOrigin,
    colorType,
    nullptr,
    &props));

  canvas_ = (*surface_)->getCanvas();
  paint_.setAntiAlias(true);
  ui_group_.emplace_back(std::move(std::make_unique<UIGroup>(UIAttributes{0, 0, 1, -1}, "root")));
  basic_ui_ = {ui_group_, ui_group_.end() - 1};
}
