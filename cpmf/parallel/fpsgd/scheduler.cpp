#include <condition_variable>
#include <mutex>
#include <random>
#include <thread>
#include <vector>

namespace cpmf {
namespace parallel {
namespace fpsgd {

class Scheduler {
 public:
  Scheduler(const int &num_u_blks, const int &num_i_blks, const int &num_th)
    : num_user_blocks_(num_u_blks), num_item_blocks_(num_i_blks),
      num_threads_(num_th), num_total_blocks_(num_u_blks * num_i_blks),
      processed_counts_(0), num_paused_threads_(0), num_stopped_threads_(0),
      free_user_blocks_(num_u_blks, true), free_item_blocks_(num_i_blks, true),
      processed_blocks_(num_total_blocks_, false),
      is_paused_(false), is_terminated_(false),
      engine_(rd_()) {}
  void initialize_threads(const std::shared_ptr<cpmf::common::Matrix> R,
                          std::shared_ptr<cpmf::common::Model> model);
  void pause();
  void resume();
  void terminate();

 private:
  const int num_user_blocks_, num_item_blocks_, num_threads_, num_total_blocks_;

  void process_blocks(const std::shared_ptr<cpmf::common::Matrix> R,
                      std::shared_ptr<cpmf::common::Model> model);
  int get_free_block_id();
  void after_processed(const int &block_id);
  void halt_if_paused();
  bool stop_if_terminated();

  int processed_counts_, num_paused_threads_, num_stopped_threads_;
  bool is_paused_, is_terminated_;
  std::vector<bool> free_user_blocks_, free_item_blocks_, processed_blocks_;
  std::vector<std::thread> threads_;
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

void Scheduler::pause() {
  // wait until all blocks are processed
  // and be notified by Scheduler::after_processed()
  {
    std::unique_lock<std::mutex> uniq_lk(mtx_);
    cond_.wait(uniq_lk,
               [&] { return processed_counts_ == num_total_blocks_; } );
  }
  // wait until all threads are paused
  // and be notified by Scheduler::halt_if_paused()
  {
    std::unique_lock<std::mutex> uniq_lk(mtx_);
    is_paused_ = true;
    cond_.wait(uniq_lk, [&] { return num_paused_threads_ == num_threads_; } );
  }
}

void Scheduler::resume() {
  {
    std::lock_guard<std::mutex> lock(mtx_);
    is_paused_ = false;
    processed_counts_ = 0;
    std::fill(free_user_blocks_.begin(), free_user_blocks_.end(), true);
    std::fill(free_item_blocks_.begin(), free_item_blocks_.end(), true);
    std::fill(processed_blocks_.begin(), processed_blocks_.end(), false);
  }
  // wake all waiting threads in Scheduler::halt_if_paused()
  cond_.notify_all();
}

void Scheduler::terminate() {
  std::unique_lock<std::mutex> uniq_lk(mtx_);
  is_terminated_ = true;
  // wait until all threads are stopped
  // and be notified by Scheduler::stop_if_terminated()
  // to confirm that all threads finish executing before join
  cond_.wait(uniq_lk, [&] { return num_stopped_threads_ == num_threads_; } );
  for (auto &thread : threads_) { thread.join(); }
}

void Scheduler::process_blocks(const std::shared_ptr<cpmf::common::Matrix> R,
                               std::shared_ptr<cpmf::common::Model> model) {
  while (true) {
    // this thread halts if the scheduler is paused,
    // and stops if the scheduler is terminated
    halt_if_paused();
    if (stop_if_terminated()) { break; }

    const int block_id = get_free_block_id();
    if (block_id == -1) { continue; }   // try getting a free block
    model->sgd(R->blocks[block_id]);
    after_processed(block_id);
  }
}

/**
 * if this thread can get a free block, return 1,2,3,...
 * otherwise, return -1
 */
int Scheduler::get_free_block_id() {
  std::lock_guard<std::mutex> lock(mtx_);
  // search for free blocks ('free' means no other threads are working on)
  std::vector<int> free_blocks;
  for (int ubid = 0; ubid < num_user_blocks_; ubid++) {
    if (!free_user_blocks_[ubid]) { continue; }
    for (int ibid = 0; ibid < num_item_blocks_; ibid++) {
      if (!free_item_blocks_[ibid]) { continue; }
      int block_id = ubid * num_item_blocks_ + ibid;
      if (processed_blocks_[block_id]) { continue; }
      free_blocks.push_back(block_id);
    }
  }
  if (free_blocks.empty()) { return -1; }   // cannot get a free block
  // randomly select free block
  std::uniform_int_distribution<int> distribution(0, free_blocks.size()-1);
  const int selected_bid = free_blocks[distribution(engine_)];
  free_user_blocks_[selected_bid / num_item_blocks_] = false;
  free_item_blocks_[selected_bid % num_item_blocks_] = false;
  return selected_bid;
}

void Scheduler::after_processed(const int &block_id) {
  processed_blocks_[block_id] = true;
  {
    std::lock_guard<std::mutex> lock(mtx_);
    free_user_blocks_[block_id / num_item_blocks_] = true;
    free_item_blocks_[block_id % num_item_blocks_] = true;
    // notify the first scope in Scheduler::pause()
    if (++processed_counts_ == num_total_blocks_) { cond_.notify_one(); }
  }
}

void Scheduler::halt_if_paused() {
  if (!is_paused_) { return; }

  // wait until the scheduler is resumed
  // and be notified by Scheduler::resume()
  {
    std::unique_lock<std::mutex> uniq_lk(mtx_);
    // notify the second scope in Scheduler::pause()
    if (++num_paused_threads_ == num_threads_) { cond_.notify_one(); }
    cond_.wait(uniq_lk, [&] { return !is_paused_; } );
  }
  {
    std::lock_guard<std::mutex> lock(mtx_);
    num_paused_threads_--;
  }
}

bool Scheduler::stop_if_terminated() {
  if (!is_terminated_) { return false; }
  {
    std::lock_guard<std::mutex> lock(mtx_);
    // notify Scheduler::terminate()
    if(++num_stopped_threads_ == num_threads_) { cond_.notify_one(); }
    return true;
  }
}

} // namespace fpsgd
} // namespace parallel
} // namespace cpmf
