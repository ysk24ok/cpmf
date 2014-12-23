#include <limits>
#include "fpsgd.hpp"

namespace cpmf {
namespace parallel {
namespace fpsgd {

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
  while (true) {
    const int block_id = get_free_block_id();
    model->sgd(R->blocks[block_id]);
    after_processed(block_id);
    if (is_terminated()) { break; }
  }
}

int Scheduler::ThreadPool::get_free_block_id() {
  // search for free blocks ('free' means no other threads are working on)
  std::lock_guard<std::mutex> lock(parent->mtx_);
  std::vector<int> free_blocks;
  int min_count = std::numeric_limits<int>::max();
  for (int ubid = 0; ubid < num_user_blocks_; ubid++) {
    if (!free_user_blocks_[ubid]) { continue; }
    for (int ibid = 0; ibid < num_item_blocks_; ibid++) {
      if (!free_item_blocks_[ibid]) { continue; }
      int bid = ubid * num_item_blocks_ + ibid;
      int count = processed_counts_[bid];
      if (count == min_count) {
        free_blocks.push_back(bid);
      } else if (count < min_count) {
        free_blocks.assign(1, bid);
        min_count = count;
      }
    }
  }
  // randomly select free block
  std::uniform_int_distribution<int> distribution(0, free_blocks.size()-1);
  const int selected_bid = free_blocks[distribution(engine_)];
  free_user_blocks_[selected_bid / num_item_blocks_] = false;
  free_item_blocks_[selected_bid % num_item_blocks_] = false;
  return selected_bid;
}

void Scheduler::ThreadPool::after_processed(const int &block_id) {
  {
    std::lock_guard<std::mutex> lock(parent->mtx_);
    ++processed_counts_[block_id];
    ++total_processed_counts_;
    free_user_blocks_[block_id / num_item_blocks_] = true;
    free_item_blocks_[block_id % num_item_blocks_] = true;
  }
  wait_till_resumed();
}

void Scheduler::ThreadPool::wait_till_resumed() {
  if (total_processed_counts_ < num_blocks_) { return; }
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
  // after resumed
  {
    std::lock_guard<std::mutex> lock(parent->mtx_);
    total_processed_counts_ = 0;
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

} // namespace fpsgd
} // namespace parallel
} // namespace cpmf
