#include "modules/gui/include/sdl_gui/gui/controller.h"
#include "sdl_gui/gui/button.h"


int main(int argc, char *argv[]) {
  controller.AddObject(std::make_unique<Button>(0.1, 1, 0.5, 1, 1, SDL_Color{255, 0, 0, 255}, 0.5));
  controller.StartLoop();
  return 0;
}
