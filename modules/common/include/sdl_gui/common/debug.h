#ifndef SDL_GUI_DEBUG_H
#define SDL_GUI_DEBUG_H

#include <chrono>
#include <source_location>

#include "glog/logging.h"
#define assertm(exp, msg) assert((void(msg), exp))

class Timer {
 public:
  explicit Timer(const std::source_location location = std::source_location::current())
      : start_time_(std::chrono::high_resolution_clock::now()), location_(location) {}

  ~Timer() {
    using std::chrono_literals::operator""ns;
    const double final = static_cast<double>((std::chrono::high_resolution_clock::now() - start_time_) / 1ns);
    DLOG_EVERY_N(INFO, 500) << "file: " << location_.file_name() << '(' << location_.line() << ':' << location_.column()
                            << ") `" << location_.function_name() << "`: " << final << "ns fps:" << 1e9 / final << '\n';
  }

 private:
  std::chrono::high_resolution_clock::time_point start_time_;
  std::source_location location_;
};

class FPSTimer {
 public:
  explicit FPSTimer() : start_time_(std::chrono::high_resolution_clock::now()) {}

  void Tick(const std::source_location location = std::source_location::current()) {
    count_++;
    if (count_ < 500) {
      return;
    }
    using std::chrono_literals::operator""ns;
    const double final = static_cast<double>((std::chrono::high_resolution_clock::now() - start_time_) / count_ / 1ns);
    LOG(INFO) << "file: " << location.file_name() << '(' << location.line() << ':' << location.column() << ") `"
              << location.function_name() << "`: " << final << "ns fps:" << 1e9 / final << '\n';
    count_ = 0;
    start_time_ = std::chrono::high_resolution_clock::now();
  }

 private:
  std::chrono::high_resolution_clock::time_point start_time_;
  int count_{};
};
#endif  // SDL_GUI_DEBUG_H
