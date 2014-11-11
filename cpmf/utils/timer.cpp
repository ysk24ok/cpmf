#include <iostream>
#include "utils.hpp"

namespace cpmf {
namespace utils {

void Timer::start(std::string const &msg) {
  if (msg != "") std::cout << msg << std::endl;
  start_time = std::chrono::high_resolution_clock::now();
  restart_time = start_time;
  duration = std::chrono::duration_cast<std::chrono::microseconds>
               (start_time - start_time);
}

float Timer::pause(std::string const &msg) {
  if (msg != "") std::cout << msg << std::endl;
  duration += std::chrono::duration_cast<std::chrono::microseconds>
               (std::chrono::high_resolution_clock::now() - restart_time);
  return (float) duration.count() / 1000000;
}

void Timer::resume(std::string const &msg) {
  if (msg != "") std::cout << msg << std::endl;
  restart_time = std::chrono::high_resolution_clock::now();
}

float Timer::stop(std::string const &msg) {
  if (msg != "") std::cout << msg << std::endl;
  auto whole_elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>
                         (std::chrono::high_resolution_clock::now() - start_time);
  return (float) whole_elapsed_time.count() / 1000000;
}


} // namespace utils
} // namespace cpmf
