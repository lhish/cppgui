#include "modules/gui/include/sdl_gui/controller.h"

auto &controller = Controller::GetInstance();

int main(int argc, char *argv[]) {
  // controller.AddObject(std::make_unique<Button>(Button{0.1, 0.1, 0.5, 0.5, 1, {255, 0, 0, 255}, 0.5}));
  controller.StartLoop();
  return 0;
}
