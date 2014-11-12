#ifndef CPMF_UTILS_HPP_
#define CPMF_UTILS_HPP_

#include <chrono>
#include <string>

namespace cpmf {
namespace utils {

class Logger {
 public:
  Logger() : basic_blank_("   "), time_sec_suffix_(" sec") {}
  void put_msg(std::string const &msg);
  void put_msg_with_time(std::string const &msg, float const &time_sec);
  void put_table_header(const char * id, int num, ...);
  void put_table_row(int id, int num, ...);
 private:
  std::string basic_blank_;
  std::string time_sec_suffix_;
};

class Timer {
  public:
    Timer() : logger(Logger()) {}
    void start(std::string const &msg = "");
    float pause(std::string const &msg = "");
    void resume(std::string const &msg = "");
    float stop(std::string const &msg = "");
  private:
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point restart_time;
    std::chrono::microseconds duration;
    Logger logger;
};

} // namespace utils
} // namespace cpmf

#endif
