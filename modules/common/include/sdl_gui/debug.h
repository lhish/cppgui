#ifndef SDL_GUI_DEBUG_H
#define SDL_GUI_DEBUG_H

#include <iostream>

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

#endif //SDL_GUI_DEBUG_H