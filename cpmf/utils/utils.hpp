#ifndef CPMF_UTILS_HPP_
#define CPMF_UTILS_HPP_

#include <chrono>
#include <string>

namespace cpmf {
namespace utils {

class Timer {
  public:
    void start(std::string const &msg = "");
    float pause(std::string const &msg = "");
    void resume(std::string const &msg = "");
    float stop(std::string const &msg = "");
  private:
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point restart_time;
    std::chrono::microseconds duration;
};

} // namespace utils
} // namespace cpmf

#endif
