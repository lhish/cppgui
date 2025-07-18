#include <exception>
#include <functional>
#include <iostream>
#include <set>
#include <source_location>
#include <string>
#include <sstream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include"include/refl_s.h"
#include <include/core/SkCanvas.h>
#include <include/core/SkFont.h>
#include <include/core/SkFontMgr.h>
#include <include/core/SkFontStyle.h>
#include <include/core/SkPaint.h>
#include <include/core/SkSurfaceProps.h>
#include <include/gpu/ganesh/GrBackendSurface.h>
#include <include/gpu/ganesh/GrDirectContext.h>
#include <include/gpu/ganesh/GrRecordingContext.h>
#include <include/gpu/ganesh/SkSurfaceGanesh.h>
#include <include/gpu/ganesh/gl/GrGLBackendSurface.h>
#include <include/gpu/ganesh/gl/GrGLDirectContext.h>
#include <include/gpu/ganesh/gl/GrGLInterface.h>
#include <include/ports/SkFontMgr_mac_ct.h>
#include <src/gpu/ganesh/gl/GrGLUtil.h>
#include "include/core/SkColorSpace.h"
#include "SDL3/SDL_opengl.h"


class Debug {
  public:
    template<typename... Args>
    Debug operator,(Args &&... other) {
      std::cout << (other << ...);
      std::cout << std::endl;
      return *this;
    }
};

inline Debug &debug() {
  static Debug debug;
  return debug;
}

void handle_error(const std::source_location location = std::source_location::current()) {
  std::stringstream output;
  output << "file: "
      << location.file_name() << '('
      << location.line() << ':'
      << location.column() << ") `"
      << location.function_name() << "`: ";
  SDL_Log(output.str().c_str(), SDL_GetError());
  std::terminate();
}

/**
 * @brief 检查 SDL 操作的返回值。如果指针为空，记录错误并终止程序。
 * @param pointer 要检查的指针。
 * @param location 调用此函数时的源位置信息。
 * @return 传入的指针（如果非空）。
 */
auto CheckSDL(auto pointer,
              const std::source_location location =
                  std::source_location::current()) {
  if (!pointer) {
    handle_error(location);
  }
  return pointer;
}

class UI {
  friend class UIRef;

  public:
    virtual ~UI() = default;

    explicit UI() = default;

    UI(const float x, const float y, const float w, const float h, const int depth)
      : x_(x),
        y_(y),
        w_(w),
        h_(h), depth_(depth) {
    }

    virtual void Draw(float offset_x = 0, float offset_y = 0, float offset_zoom_rate = 1) {
    };

    virtual void Click() {
    };

    friend bool operator==(const UI &lhs, const UI &rhs) {
      return lhs.depth_ == rhs.depth_;
    }

    friend bool operator!=(const UI &lhs, const UI &rhs) {
      return !(lhs == rhs);
    }

    friend bool operator<(const UI &lhs, const UI &rhs) {
      return lhs.depth_ < rhs.depth_;
    }

    friend bool operator<=(const UI &lhs, const UI &rhs) {
      return !(rhs < lhs);
    }

    friend bool operator>(const UI &lhs, const UI &rhs) {
      return rhs < lhs;
    }

    friend bool operator>=(const UI &lhs, const UI &rhs) {
      return !(lhs < rhs);
    }

  protected:
    float x_{}, y_{}; //0-1
    float w_{}, h_{}; //0-1
    int depth_{}; //>=0
    float zoom_rate_{1};
};

REFL(UI, x_, y_, w_, h_, depth_, zoom_rate_)

class UIRef {
  public:
    UIRef(std::vector<std::unique_ptr<UI> > &base_ui_vec,
          const std::vector<std::unique_ptr<UI> >::iterator it): base_ui_vec_(base_ui_vec),
                                                                 index_(it - base_ui_vec.begin()) {
    }

    UI &operator*() const {
      return *base_ui_vec_[index_];
    }

    UI *operator->() const {
      return base_ui_vec_[index_].get();
    }

    explicit operator UI &() const {
      return *base_ui_vec_[index_];
    }

    explicit operator bool() const {
      return index_ < base_ui_vec_.size();
    }

    friend bool operator==(const UIRef &lhs, const UIRef &rhs) {
      return lhs.index_ == rhs.index_;
    }

    friend bool operator!=(const UIRef &lhs, const UIRef &rhs) {
      return !(lhs == rhs);
    }

    friend bool operator<(const UIRef &lhs, const UIRef &rhs) {
      if (lhs.base_ui_vec_[lhs.index_]->depth_ != rhs.base_ui_vec_[rhs.index_]->depth_) {
        return lhs.base_ui_vec_[lhs.index_]->depth_ < rhs.base_ui_vec_[rhs.index_]->depth_;
      }
      return lhs.index_ < rhs.index_;
    }

    friend bool operator<=(const UIRef &lhs, const UIRef &rhs) {
      return !(rhs < lhs);
    }

    friend bool operator>(const UIRef &lhs, const UIRef &rhs) {
      return rhs < lhs;
    }

    friend bool operator>=(const UIRef &lhs, const UIRef &rhs) {
      return !(lhs < rhs);
    }

  private:
    std::vector<std::unique_ptr<UI> > &base_ui_vec_;
    size_t index_;
};

class UIGroup : public UI {
  public:
    UIGroup(const float x,
            const float y,
            const float w,
            const float h,
            const int depth,
            const std::set<UIRef> &ui_group) : UI(x, y, w, h, depth),
                                               ui_group_(ui_group) {
    }

    void Draw(const float offset_x, const float offset_y, const float offset_zoom_rate) override {
      for (auto &ui : ui_group_) {
        ui->Draw(x_ + offset_x, y_ + offset_y, offset_zoom_rate * zoom_rate_);
      }
    }

    template<typename... Args>
    void AddObject(Args &&... args) {
      ui_group_.emplace(std::forward<Args>(args)...);
    }

    ~UIGroup() override = default;

    std::set<UIRef> ui_group_;
};

class Controller {
  public:
    static Controller &GetInstance() {
      static Controller controller;
      return controller;
    }

    void handle_events() {
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
        switch (event.type) {
          // case SDL_EVENT_MOUSE_MOTION:
          //   if (event.motion.state == true) {
          //     SkRect &rect = state->fRects.back();
          //     rect.fRight = event.motion.x;
          //     rect.fBottom = event.motion.y;
          //   }
          //   break;
          // case SDL_EVENT_MOUSE_BUTTON_DOWN:
          //   if (event.button.down == true) {
          //     state->fRects.push_back() = SkRect::MakeLTRB(SkIntToScalar(event.button.x),
          //                                                  SkIntToScalar(event.button.y),
          //                                                  SkIntToScalar(event.button.x),
          //                                                  SkIntToScalar(event.button.y));
          //   }
          //   break;
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

    void Draw() {
      canvas_->clear(SK_ColorWHITE);
      canvas_->translate(0, static_cast<float>(ori_height_ - height_));
      canvas_->translate(0, static_cast<float>(-(ori_height_ - height_)));
    }

    void StartLoop() {
      while (keep_going) {
        handle_events();
        Draw();
        if (auto dContext = GrAsDirectContext(canvas_->recordingContext())) {
          dContext->flushAndSubmit();
        }
        SDL_GL_SwapWindow(window_);
      }
    }

    ~Controller() {
      SDL_GL_DestroyContext(glContext_);
      SDL_DestroyWindow(window_);
      SDL_Quit();
    }

    [[nodiscard]] std::pair<float, float> GetWindowSize() const {
      int width, height;
      SDL_GetWindowSize(window_, &width, &height);
      return {width, height};
    }

    void AddObject(std::unique_ptr<UI> ui) {
      ui_group_.emplace_back(std::move(ui));
      basic_ui_.AddObject(ui_group_, ui_group_.end() - 1);
    }

  private:
    Controller() {
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
      // canvas->scale(static_cast<float>(dm->w) / dm->w, static_cast<float>(dm->h)/dm->h);
      // sk_sp<SkTypeface> typeface(SkFontMgr_New_CoreText(nullptr)->legacyMakeTypeface("", SkFontStyle()));
      // SkFont font(typeface);
    }

    uint32_t windowFlags{};
    SDL_GLContext glContext_{};
    SkCanvas *canvas_;
    SDL_Event event_{};
    SDL_Window *window_;
    UIGroup basic_ui_{0, 0, 1, 1, 0, {}};
    bool keep_going = true;
    std::vector<std::unique_ptr<UI> > ui_group_{};
    int width_;
    int ori_width_;
    int height_;
    int ori_height_;
    SDL_DisplayID *displays_;
    const SDL_DisplayMode *dm_;
    SkPaint paint_{};
    std::unique_ptr<sk_sp<SkSurface> > surface_;
};

auto &controller = Controller::GetInstance();

class Button : public UI {
  public:
    Button(const float x,
           const float y,
           const float w,
           const float h,
           const int depth,
           const SDL_Color &color,
           const float radius_ratio)
      : UI(x, y, w, h, depth),
        color_(color),
        radius_ratio_(radius_ratio) {
    }

    void Draw(const float offset_x, const float offset_y, const float offset_zoom_rate) override {
    }

  private:
    SDL_Color color_{255, 0, 0, 255};
    float radius_ratio_{};
};

/**
 * @brief 程序主函数。
 * @param argc 命令行参数数量。
 * @param argv 命令行参数数组。
 * @return 程序的退出码。
 */
int main(int argc, char *argv[]) {
  // controller.AddObject(std::make_unique<Button>(Button{0.1, 0.1, 0.5, 0.5, 1, {255, 0, 0, 255}, 0.5}));
  controller.StartLoop();
  return 0;
}
