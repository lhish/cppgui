#include "sdl_gui/gui/controller.h"

#include <variant>
#include "../../common/include/sdl_gui/common/sdl_utils.h"
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

#include "SDL3/SDL_opengl.h"

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
        }
        break;
      case SDL_EVENT_KEY_DOWN: {
        SDL_Keycode key = event.key.key;
        if (key == SDLK_ESCAPE) {
          keep_going = false;
        }
        break;
      }
      case SDL_EVENT_QUIT:
        keep_going = false;
        break;
      default:
        break;
    }
  }
}

void Controller::Draw() {
  canvas_->clear(SK_ColorWHITE);
  canvas_->translate(0, static_cast<float>(ori_height_ - height_));
  basic_ui_.Draw(0, 0, 1);
  canvas_->translate(0, static_cast<float>(-(ori_height_ - height_)));
}

void Controller::StartLoop() {
  while (keep_going) {
    handle_events();
    Draw();
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

void Controller::AddObject(std::unique_ptr<UI> ui) {
  ui_group_.emplace_back(std::move(ui));
  basic_ui_.AddObject(ui_group_, ui_group_.end() - 1);
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
}
