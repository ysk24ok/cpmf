#include "line_based.hpp"

namespace cpmf {
namespace parallel {
namespace line_based {

void Scheduler::start(const std::shared_ptr<cpmf::common::Matrix> R,
                      std::shared_ptr<cpmf::common::Model> model) {
  tp.set_parent_ptr(this);
  tp.initialize(R, model);
}

void Scheduler::wait_for_all_blocks_processed() {
  // be notified from ThreadPool::wait_till_resumed()
  std::unique_lock<std::mutex> uniq_lk(mtx_);
  cond_.wait(uniq_lk, [&] { return is_paused_; } );
}

void Scheduler::resume() {
  is_paused_ = false;
  cond_.notify_all(); // notify ThreadPool::wait_till_resumed()
}

void Scheduler::terminate() {
  std::unique_lock<std::mutex> uniq_lk(mtx_);
  is_terminated_ = true;
  cond_.wait(uniq_lk, [&] { return tp.all_threads_stopped; } );
}

} // namespace line_based
} // namespace parallel
} // namespace cpmf
