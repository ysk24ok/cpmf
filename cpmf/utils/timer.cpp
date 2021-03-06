#include "utils.hpp"

using namespace std::chrono;

namespace cpmf {
namespace utils {

void Timer::start(const std::string &msg) {
  start_time_ = high_resolution_clock::now();
  restart_time_ = start_time_;
  duration_ = duration_cast<microseconds>(start_time_ - start_time_);
  logger_.put_msg(msg);
}

float Timer::pause(const std::string &msg) {
  duration_ += duration_cast<microseconds>
                 (high_resolution_clock::now() - restart_time_);
  float dur_sec = (float) duration_.count() / 1000000;
  logger_.put_msg_with_time(msg, dur_sec);
  return dur_sec;
}

void Timer::resume(const std::string &msg) {
  restart_time_ = high_resolution_clock::now();
  logger_.put_msg(msg);
}

float Timer::stop(const std::string &msg) {
  microseconds time_from_start = duration_cast<microseconds>
                                   (high_resolution_clock::now() - start_time_);
  float tfs_sec = (float) time_from_start.count() / 1000000;
  logger_.put_msg_with_time(msg, tfs_sec);
  return tfs_sec;
}

} // namespace utils
} // namespace cpmf
