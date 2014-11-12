#include <iostream>
#include "utils.hpp"

namespace cpmf {
namespace utils {

void Logger::log_msg(std::string const &msg) {
  if (msg != "") { std::cout << msg << std::endl; }
}

void Logger::log_msg_with_time(std::string const &msg, float const &time_sec) {
  if (msg == "") { return; }
  std::string t = "(" + std::to_string(time_sec) + time_sec_suffix_ + ")";
  std::cout << msg + basic_blank_ + t << std::endl;
}

} // namespace utils
} // namespace cpmf
