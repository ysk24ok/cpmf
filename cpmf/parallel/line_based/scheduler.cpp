#include <condition_variable>
#include <mutex>
#include <random>
#include <thread>
#include <vector>

namespace cpmf {
namespace parallel {
namespace line_based {

class Scheduler {
 public:
  Scheduler(const int &num_u_blks, const int &num_i_blks, const int &num_th)
    : num_user_blocks_(num_u_blks), num_item_blocks_(num_i_blks),
      num_threads_(num_th), num_total_blocks_(num_u_blks * num_i_blks),
      processed_counts_(0), num_paused_threads_(0), num_stopped_threads_(0),
      is_paused_(false), is_terminated_(false),
      engine_(rd_()) {}
  void initialize_threads(const std::shared_ptr<cpmf::common::Matrix> R,
                          std::shared_ptr<cpmf::common::Model> model);
  void wait_for_all_blocks_processed();
  void resume();
  void terminate();

 private:
  const int num_user_blocks_, num_item_blocks_, num_threads_, num_total_blocks_;

  void process_blocks(const std::shared_ptr<cpmf::common::Matrix> R,
                      std::shared_ptr<cpmf::common::Model> model);
  void wait_for_other_threads(const int &block_id);
  void halt(const int &thread_id);
  bool stop_if_terminated();
  int get_thread_id(std::thread::id native_id);

  int processed_counts_, num_paused_threads_, num_stopped_threads_;
  bool is_paused_, is_terminated_;
  std::vector<std::thread> threads_;
  std::vector<std::thread::id> thread_ids_;
  std::mutex mtx_;
  std::condition_variable cond_;
  std::random_device rd_;
  std::mt19937 engine_;
};

void Scheduler::initialize_threads(
    const std::shared_ptr<cpmf::common::Matrix> R,
    std::shared_ptr<cpmf::common::Model> model) {
  // XXX: error occurs when using threads_.resize
  for (int i = 0; i < num_threads_; i++) {
   threads_.push_back(std::thread(&Scheduler::process_blocks, this, R, model));
  }
}

void Scheduler::wait_for_all_blocks_processed() {
  // Wait until all threads are woken up by Scheduler::resume()
  // Otherwise, this iteration will be finished with no blocks processed.
  {
    std::unique_lock<std::mutex> uniq_lk(mtx_);
    cond_.wait(uniq_lk, [&] { return num_paused_threads_ == 0; } );
  }
  // Wait until be notified by Scheduler::halt()
  {
    std::unique_lock<std::mutex> uniq_lk(mtx_);
    cond_.wait(uniq_lk, [&] { return num_paused_threads_ == num_threads_; } );
  }
  is_paused_ = true;
}

void Scheduler::resume() {
  processed_counts_ = 0;
  is_paused_ = false;
  // Wake all waiting threads up in Scheduler::halt()
  cond_.notify_all();
}

void Scheduler::terminate() {
  is_terminated_ = true;
  cond_.notify_all();
  // Wait until all threads are stopped
  // and be notified by Scheduler::stop_if_terminated()
  // to confirm that all threads finish executing before join
  {
    std::unique_lock<std::mutex> uniq_lk(mtx_);
    cond_.wait(uniq_lk, [&] { return num_stopped_threads_ == num_threads_; } );
  }
  for (auto &thread : threads_) { thread.join(); }
}

void Scheduler::process_blocks(const std::shared_ptr<cpmf::common::Matrix> R,
                               std::shared_ptr<cpmf::common::Model> model) {
  const int thread_id = get_thread_id(std::this_thread::get_id());
  while(true) {
    int uid = thread_id - 1;
    int bid = uid * num_item_blocks_ + (thread_id - 1);
    for (int i = 0, total = num_user_blocks_ / num_threads_; i < total; i++) {
      for (int j = 0; j < num_item_blocks_; j++) {
        model->sgd(R->blocks[bid]);
        wait_for_other_threads(bid);
        if (stop_if_terminated()) { return; }
        if (j == num_item_blocks_ - 1) { break; }
        // assign the next block except in the last subiteration
        if (++bid / num_item_blocks_ != uid) { bid -= num_item_blocks_; }
      }
      if (i == total - 1) { break; }
      uid += num_threads_;
      bid += num_threads_ * num_item_blocks_;
    }
    halt(thread_id);
  }
}

void Scheduler::wait_for_other_threads(const int &block_id) {
  // XXX: sometimes the program will stops without waiting up all threads
  std::unique_lock<std::mutex> uniq_lk(mtx_);
  if (++processed_counts_ % num_threads_ == 0) { cond_.notify_all(); }
  cond_.wait(uniq_lk, [&] { return processed_counts_ % num_threads_ == 0; } );
}

void Scheduler::halt(const int &thread_id) {
  if (!is_paused_) {
    std::lock_guard<std::mutex> lock(mtx_);
    is_paused_ = true;
  }
  {
    std::lock_guard<std::mutex> lock(mtx_);
    // TODO: notify_one() wakes up any thread which is waiting
    // and programmers cannot designate which thread to wake up.
    // That means some child threads ( which processed blocks ) may be woken up.
    // Thus, we use notify_all() instead of notify_one()
    if (++num_paused_threads_ == num_threads_) { cond_.notify_all(); }
  }
  // All threads wait until be notified by Scheduler::resume()
  {
    std::unique_lock<std::mutex> uniq_lk(mtx_);
    cond_.wait(uniq_lk, [&] { return !is_paused_; } );
  }
  // Notify Scheduler::wait_for_all_blocks_processed() after all threads wake up
  {
    std::lock_guard<std::mutex> lock(mtx_);
    if (--num_paused_threads_ == 0) { cond_.notify_one(); }
  }
}

bool Scheduler::stop_if_terminated() {
  if (!is_terminated_) { return false; }
  {
    std::lock_guard<std::mutex> lock(mtx_);
    // notify Scheduler::terminate()
    if (++num_stopped_threads_ == num_threads_) { cond_.notify_one(); }
    return true;
  }
}

int Scheduler::get_thread_id(std::thread::id native_id) {
  std::lock_guard<std::mutex> lock(mtx_);
  thread_ids_.push_back(native_id);
  return thread_ids_.size();
}


} // namespace line_based
} // namespace parallel
} // namespace cpmf
