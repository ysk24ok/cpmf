#include "line_based.hpp"

namespace cpmf {
namespace parallel {
namespace line_based {

Scheduler::ThreadPool::~ThreadPool() {
  for (auto &thread : threads_) { thread.join(); }
}

void Scheduler::ThreadPool::set_parent_ptr(Scheduler * scheduler) {
  parent = scheduler;
}

void Scheduler::ThreadPool::initialize(
    const std::shared_ptr<cpmf::common::Matrix> R,
    std::shared_ptr<cpmf::common::Model> model) {
  for (int i = 0; i < num_threads_; i++) {
    threads_[i] = std::thread(&Scheduler::ThreadPool::run, this, R, model);
  }
}

void Scheduler::ThreadPool::run(const std::shared_ptr<cpmf::common::Matrix> R,
                                std::shared_ptr<cpmf::common::Model> model) {
  const int thread_id = get_thread_id(std::this_thread::get_id());
  int uid = thread_id - 1;
  int iid = thread_id - 1;
  int bid;
  while(true) {
    for (int i = 1, total = num_user_blocks_ / num_threads_; i <= total; i++) {
      for (int j = 1; j <= num_item_blocks_; j++) {
        bid = uid * num_item_blocks_ + iid;
        model->sgd(R->blocks[bid]);
        if (is_terminated()) { return; }
        wait_for_other_threads();
        // assign the next block except the last subiteration
        if (j == num_item_blocks_) { break; }
        if (++iid >= num_item_blocks_) { iid -= num_item_blocks_; }
      }
      uid += num_threads_;
      if (uid >= num_user_blocks_) { uid -= num_user_blocks_; }
    }
    wait_till_resumed();
  }
}

void Scheduler::ThreadPool::wait_for_other_threads() {
  std::unique_lock<std::mutex> uniq_lk(parent->mtx_);
  if (++processed_counts_ % num_threads_ == 0) { parent->cond_.notify_all(); }
  parent->cond_.wait(uniq_lk,
                     [&] { return processed_counts_ % num_threads_ == 0; } );
}

void Scheduler::ThreadPool::wait_till_resumed() {
  // notify Scheduler::wait_for_all_blocks_processed()
  {
    std::lock_guard<std::mutex> lock(parent->mtx_);
    parent->is_paused_ = true;
    parent->cond_.notify_one();
  }
  // be notified from Scheduler::resume()
  {
    std::unique_lock<std::mutex> uniq_lk(parent->mtx_);
    parent->cond_.wait(uniq_lk, [&] { return !parent->is_paused_; } );
  }
}

bool Scheduler::ThreadPool::is_terminated() {
  if (!parent->is_terminated_) { return false; }
  {
    std::lock_guard<std::mutex> lock(parent->mtx_);
    // notify Scheduler::terminate()
    if (++num_stopped_threads_ == num_threads_) {
      all_threads_stopped = true;
      parent->cond_.notify_one();
    }
  }
  return true;
}

int Scheduler::ThreadPool::get_thread_id(std::thread::id native_id) {
  std::lock_guard<std::mutex> lock(parent->mtx_);
  thread_ids_.push_back(native_id);
  return thread_ids_.size();
}


} // namespace line_based
} // namespace parallel
} // namespace cpmf
