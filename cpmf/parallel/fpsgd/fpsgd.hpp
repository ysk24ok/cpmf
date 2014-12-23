#ifndef CPMF_PARALLEL_FPSGD_HPP_
#define CPMF_PARALLEL_FPSGD_HPP_

#include <condition_variable>
#include <memory>
#include <mutex>
#include <random>
#include <thread>
#include <vector>

#include <cpmf/common/common.hpp>

namespace cpmf {
namespace parallel {
namespace fpsgd {

class Scheduler {

  class ThreadPool {
   public:
    ThreadPool(const int &num_u_blks, const int &num_i_blks,
               const int &num_thrs)
      : all_threads_stopped(false),
        num_user_blocks_(num_u_blks), num_item_blocks_(num_i_blks),
        num_threads_(num_thrs), num_blocks_(num_u_blks * num_i_blks),
        total_processed_counts_(0), num_stopped_threads_(0),
        free_user_blocks_(num_u_blks, true),
        free_item_blocks_(num_i_blks, true),
        processed_counts_(num_u_blks * num_i_blks, 0),
        threads_(num_thrs), engine_(rd_()) {}
    ~ThreadPool();
    void set_parent_ptr(Scheduler * scheduler);
    void initialize(const std::shared_ptr<cpmf::common::Matrix> R,
                    std::shared_ptr<cpmf::common::Model> model);

    bool all_threads_stopped;

   private:
    void run(const std::shared_ptr<cpmf::common::Matrix> R,
             std::shared_ptr<cpmf::common::Model> model);
    int get_free_block_id();
    void after_processed(const int &block_id);
    void wait_till_resumed();
    bool is_terminated();

    const int num_user_blocks_, num_item_blocks_, num_threads_, num_blocks_;
    int total_processed_counts_, num_stopped_threads_;
    std::vector<bool> free_user_blocks_, free_item_blocks_;
    std::vector<int> processed_counts_;
    std::vector<std::thread> threads_;
    std::random_device rd_;
    std::mt19937 engine_;
    Scheduler * parent;
  };

 public:
  Scheduler(const int &num_u_blks, const int &num_i_blks, const int &num_thrs)
    : is_paused_(false), is_terminated_(false),
      tp(num_u_blks, num_i_blks, num_thrs) {}
  void start(const std::shared_ptr<cpmf::common::Matrix> R,
             std::shared_ptr<cpmf::common::Model> model);
  void wait_for_all_blocks_processed();
  void resume();
  void terminate();

 private:
  bool is_paused_, is_terminated_;
  std::mutex mtx_;
  std::condition_variable cond_;
  ThreadPool tp;
};

} // namespace fpsgd
} // namespace parallel
} // namespace cpmf

#endif // CPMF_PARALLEL_FPSGD_HPP_
