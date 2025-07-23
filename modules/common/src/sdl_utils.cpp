#include "sdl_gui/common/sdl_utils.h"

#include <SDL3/SDL.h>

#include <exception>
#include <source_location>
#include <sstream>

void handle_error(const std::source_location location) {
  std::stringstream output;
  output << "file: " << location.file_name() << '(' << location.line() << ':' << location.column() << ") `"
         << location.function_name() << "`: ";
  SDL_Log(output.str().c_str(), SDL_GetError());
  std::terminate();
}
