#ifndef CPMF_UTILS_HPP_
#define CPMF_UTILS_HPP_

#include <chrono>
#include <string>

namespace cpmf {
namespace utils {

class Logger {
 public:
  Logger() : basic_blank_("   "), time_sec_suffix_(" sec") {}

  void put_msg(const std::string &msg);
  void put_msg_with_time(const std::string &msg, const float &time_sec);
  void put_table_header(const char * id, const int &num, ...);
  void put_table_row(const int &id, const int &num, ...);
 private:
  std::string basic_blank_;
  std::string time_sec_suffix_;
};

class Timer {
 public:
  Timer() : logger_(Logger()) {}

  void start(const std::string &msg = "");
  float pause(const std::string &msg = "");
  void resume(const std::string &msg = "");
  float stop(const std::string &msg = "");
 private:
  std::chrono::high_resolution_clock::time_point start_time_;
  std::chrono::high_resolution_clock::time_point restart_time_;
  std::chrono::microseconds duration_;
  Logger logger_;
};

} // namespace utils
} // namespace cpmf

#endif
