#ifndef SDL_GUI_SDL_UTILS_H
#define SDL_GUI_SDL_UTILS_H

#include <source_location>

void handle_error(std::source_location location = std::source_location::current());

auto CheckSDL(auto pointer, const std::source_location location = std::source_location::current()) {
  if (!pointer) {
    handle_error(location);
  }
  return pointer;
}
#endif  // SDL_GUI_SDL_UTILS_H
