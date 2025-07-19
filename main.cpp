#include "modules/gui/include/sdl_gui/gui/controller.h"
#include "modules/gui/include/sdl_gui/gui/button.h"
#include "modules/gui/include/sdl_gui/gui/ui_group.h"
#include <set>


int main(int argc, char *argv[]) {
    // 顶部导航栏区域
    auto top_bar = controller.AddObject(
        std::make_unique<UIGroup>(UIAttributes{0.0, 0.0, 1.0, 0.04}, 1, std::set<UIRef>{}));

    controller.AddObject(
        std::make_unique<Button>(UIAttributes{0.05, 0.01, 0.15, 0.2},
                                 2,
                                 SDL_Color{100, 100, 255, 255},
                                 0.5),
        top_bar); // 导航按钮1
    controller.AddObject(
        std::make_unique<Button>(UIAttributes{0.25, 0.01, 0.15, 0.2},
                                 2,
                                 SDL_Color{100, 255, 100, 255},
                                 0.5),
        top_bar); // 导航按钮2
    controller.AddObject(
        std::make_unique<Button>(UIAttributes{0.8, 0.01, 0.15, 0.2},
                                 2,
                                 SDL_Color{255, 100, 100, 255},
                                 0.5),
        top_bar); // 设置按钮

    // 内容区域 (占据屏幕大部分)
    auto content_area = controller.AddObject(
        std::make_unique<UIGroup>(UIAttributes{0.0, 0.04, 1.0, -1}, 1, std::set<UIRef>{}));
    //
    // 内容区左侧面板
    auto left_panel = controller.AddObject(
        std::make_unique<UIGroup>(UIAttributes{0.0, 0.0, 0.1, -1}, 2, std::set<UIRef>{}),
        content_area);
    controller.AddObject(
        std::make_unique<Button>(UIAttributes{0.02, 0.1, 0.05, 0.5}, 3, SDL_Color{200, 200, 0, 255}, 0.5),
        left_panel); // 左侧菜单项1
    controller.AddObject(
        std::make_unique<Button>(UIAttributes{0.02, 0.2, 0.05, 0.5}, 3, SDL_Color{0, 200, 200, 255}, 0.5),
        left_panel); // 左侧菜单项2
    // 底部状态栏区域
    auto bottom_bar = controller.AddObject(
        std::make_unique<UIGroup>(UIAttributes{0.0, -0.04, 1.0, -1}, 1, std::set<UIRef>{})); // 使用负y表示从底部开始
    controller.AddObject(
        std::make_unique<Button>(UIAttributes{0.05, 0.01, 0.2, -1}, 2, SDL_Color{255, 150, 50, 255}, 0.5),
        bottom_bar); // 状态信息1
    controller.AddObject(
        std::make_unique<Button>(UIAttributes{0.75, 0.01, 0.2, -1}, 2, SDL_Color{50, 255, 150, 255}, 0.5),
        bottom_bar); // 状态信息2

    controller.StartLoop();
    return 0;
}
