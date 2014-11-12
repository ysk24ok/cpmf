#include "utils.hpp"

using namespace std::chrono;

namespace cpmf {
namespace utils {

void Timer::start(std::string const &msg) {
  start_time = high_resolution_clock::now();
  restart_time = start_time;
  duration = duration_cast<microseconds>(start_time - start_time);
  logger.put_msg(msg);
}

float Timer::pause(std::string const &msg) {
  duration += duration_cast<microseconds>(high_resolution_clock::now() - restart_time);
  float dur_sec = (float) duration.count() / 1000000;
  logger.put_msg_with_time(msg, dur_sec);
  return dur_sec;
}

void Timer::resume(std::string const &msg) {
  restart_time = high_resolution_clock::now();
  logger.put_msg(msg);
}

float Timer::stop(std::string const &msg) {
  microseconds time_from_start = duration_cast<microseconds>
                                   (high_resolution_clock::now() - start_time);
  float tfs_sec = (float) time_from_start.count() / 1000000;
  logger.put_msg_with_time(msg, tfs_sec);
  return tfs_sec;
}


} // namespace utils
} // namespace cpmf
