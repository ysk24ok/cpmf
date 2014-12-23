#ifndef CPMF_PARALLEL_LINE_BASED_HPP_
#define CPMF_PARALLEL_LINE_BASED_HPP_

#include <condition_variable>
#include <mutex>
#include <random>
#include <thread>
#include <vector>

#include <cpmf/common/common.hpp>

namespace cpmf {
namespace parallel {
namespace line_based {

class Scheduler {

  class ThreadPool {
   public:
    ThreadPool(const int &num_u_blks, const int &num_i_blks,
               const int &num_thrs)
      : all_threads_stopped(false),
        num_user_blocks_(num_u_blks), num_item_blocks_(num_i_blks),
        num_threads_(num_thrs), num_blocks_(num_u_blks * num_i_blks),
        processed_counts_(0), num_stopped_threads_(0),
        threads_(num_thrs), engine_(rd_()) {}
    ~ThreadPool();
    void set_parent_ptr(Scheduler * scheduler);
    void initialize(const std::shared_ptr<cpmf::common::Matrix> R,
                    std::shared_ptr<cpmf::common::Model> model);

    bool all_threads_stopped;

   private:
    void run(const std::shared_ptr<cpmf::common::Matrix> R,
             std::shared_ptr<cpmf::common::Model> model);
    void wait_for_other_threads();
    void wait_till_resumed();
    bool is_terminated();
    int get_thread_id(std::thread::id native_id);

    const int num_user_blocks_, num_item_blocks_, num_threads_, num_blocks_;
    int processed_counts_, num_stopped_threads_;
    std::vector<std::thread> threads_;
    std::vector<std::thread::id> thread_ids_;
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

} // namespace line_based
} // namespace parallel
} // namespace cpmf

#endif // CPMF_PARALLEL_LINE_BASED_HPP_
