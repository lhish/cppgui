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
#include <SDL3_ttf/SDL_ttf.h>

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

/**
 * @brief 检查 SDL 操作的返回值。如果指针为空，记录错误并终止程序。
 * @param pointer 要检查的指针。
 * @param location 调用此函数时的源位置信息。
 * @return 传入的指针（如果非空）。
 */
auto *CheckSDL(auto *pointer,
               const std::source_location location =
                   std::source_location::current()) {
  if (!pointer) {
    std::stringstream output;
    output << "file: "
        << location.file_name() << '('
        << location.line() << ':'
        << location.column() << ") `"
        << location.function_name() << "`: ";
    SDL_Log(output.str().c_str(), SDL_GetError());
    std::terminate();
  }
  return pointer;
}

bool CheckSDL(bool flag,
              const std::source_location location =
                  std::source_location::current()) {
  if (!flag) {
    std::stringstream output;
    output << "file: "
        << location.file_name() << '('
        << location.line() << ':'
        << location.column() << ") `"
        << location.function_name() << "`: ";
    SDL_Log(output.str().c_str(), SDL_GetError());
    std::terminate();
  }
  return flag;
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

    void StartLoop() {
      bool keep_going = true;

      // 主事件循环
      while (keep_going) {
        SDL_WaitEvent(&event_);

        switch (event_.type) {
          case SDL_EVENT_QUIT: {
            // 退出事件
            keep_going = false;
            break;
          }
          case SDL_EVENT_WINDOW_EXPOSED: {
            // 窗口暴露事件
            CheckSDL(SDL_SetRenderDrawColor(renderer_, 30, 30, 30, 255));
            CheckSDL(SDL_RenderClear(renderer_));
            basic_ui_.Draw(0, 0, 1);
            CheckSDL(SDL_RenderPresent(renderer_));
            break;
          }
        }
        SDL_Log("Event: %d", event_.type);
      }
    }

    ~Controller() {
      SDL_DestroyRenderer(renderer_);
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

    SDL_Renderer *renderer_;

  private:
    Controller() {
      SDL_SetLogPriorities(SDL_LOG_PRIORITY_VERBOSE);

      CheckSDL(SDL_Init(SDL_INIT_VIDEO));

      window_ =
          CheckSDL(SDL_CreateWindow("Hello, SDL3!", 800, 600, SDL_WINDOW_RESIZABLE));

      SDL_SetWindowPosition(window_, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

      renderer_ = CheckSDL(SDL_CreateRenderer(window_, nullptr));
    }

    SDL_Event event_{};
    SDL_Window *window_;
    UIGroup basic_ui_{0, 0, 1, 1, 0, {}};
    std::vector<std::unique_ptr<UI> > ui_group_{};
};

auto &controller = Controller::GetInstance();

int SDL_RenderFillCircle(const float x, const float y, const float radius, SDL_Color color) {
  CheckSDL(SDL_SetRenderDrawColor(controller.renderer_, color.r, color.g, color.b, color.a));
  int offset_x = 0;
  int offset_y = radius - 1;
  int d = radius - 2;
  int status = 0;

  while (offset_y >= offset_x) {
    status += CheckSDL(SDL_RenderLine(controller.renderer_,
                                      x - offset_y,
                                      y + offset_x,
                                      x + offset_y,
                                      y + offset_x));
    status += CheckSDL(SDL_RenderLine(controller.renderer_,
                                      x - offset_x,
                                      y + offset_y,
                                      x + offset_x,
                                      y + offset_y));
    status += CheckSDL(SDL_RenderLine(controller.renderer_,
                                      x - offset_x,
                                      y - offset_y,
                                      x + offset_x,
                                      y - offset_y));
    status += CheckSDL(SDL_RenderLine(controller.renderer_,
                                      x - offset_y,
                                      y - offset_x,
                                      x + offset_y,
                                      y - offset_x));

    if (status < 0) {
      status = -1;
      break;
    }

    if (d >= 2 * offset_x) {
      d -= 2 * offset_x + 1;
      offset_x += 1;
    } else if (d < 2 * (radius - offset_y)) {
      d += 2 * offset_y - 1;
      offset_y -= 1;
    } else {
      d += 2 * (offset_y - offset_x - 1);
      offset_y -= 1;
      offset_x += 1;
    }
  }

  return status;
}

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
      CheckSDL(SDL_SetRenderDrawColor(controller.renderer_, color_.r, color_.g, color_.b, color_.a));
      const auto &[width, height] = controller.GetWindowSize();
      const float final_zoom = zoom_rate_ * offset_zoom_rate;
      const float final_w = w_ * final_zoom;
      const float final_h = h_ * final_zoom;
      const float final_w_real = final_w * width;
      const float final_h_real = final_h * height;
      const float final_x = x_ + offset_x;
      const float final_y = y_ + offset_y;
      const float final_x_real = width * final_x;
      const float final_y_real = height * final_y;
      const float radius_real_length = radius_ratio_ / 2 * std::min(final_w_real, final_h_real) * final_zoom;
      const float final_x_center_real = final_x_real + radius_real_length;
      const float final_y_center_real = final_y_real + radius_real_length;
      const float final_w_center_real = final_w_real - radius_real_length * 2;
      const float final_h_center_real = final_h_real - radius_real_length * 2;

      const SDL_FRect horizontal_center_rect{
        final_x_real,
        final_y_center_real,
        final_w_real,
        final_h_center_real,
      };
      CheckSDL(SDL_RenderFillRect(controller.renderer_, &horizontal_center_rect));
      const SDL_FRect vertical_center_rect{
        final_x_center_real,
        final_y_real,
        final_w_center_real,
        final_h_real
      };
      CheckSDL(SDL_RenderFillRect(controller.renderer_, &vertical_center_rect));
      CheckSDL(SDL_RenderFillCircle(final_x_center_real, final_y_center_real, radius_real_length, color_));
      CheckSDL(SDL_RenderFillCircle(final_x_center_real + final_w_center_real,
                                    final_y_center_real,
                                    radius_real_length,
                                    color_));
      CheckSDL(SDL_RenderFillCircle(final_x_center_real,
                                    final_y_center_real + final_h_center_real,
                                    radius_real_length,
                                    color_));
      CheckSDL(SDL_RenderFillCircle(final_x_center_real + final_w_center_real,
                                    final_y_center_real + final_h_center_real,
                                    radius_real_length,
                                    color_));
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
  controller.AddObject(std::make_unique<Button>(Button{0.1, 0.1, 0.5, 0.5, 1, {255, 0, 0, 255}, 0.5}));
  controller.StartLoop();
  return 0;
}
