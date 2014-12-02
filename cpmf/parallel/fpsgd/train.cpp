#include <memory>

#include <cpmf/common/common.hpp>
#include <cpmf/utils/utils.hpp>
#include "scheduler.cpp"

namespace cpmf {
namespace parallel {
namespace fpsgd {

void train(const std::shared_ptr<cpmf::common::Matrix> R,
           std::shared_ptr<cpmf::common::Model> model,
           const cpmf::BaseParams &base_params) {
  cpmf::utils::Timer timer;
  cpmf::utils::Logger logger;
  Scheduler scheduler(R->num_user_blocks, R->num_item_blocks,
                      base_params.num_threads);

  timer.start("Now iteration starts...");
  logger.put_table_header("iteration", 2, "time", "RMSE");
  scheduler.initialize_threads(R, model);
  for (int iter = 1; iter <= base_params.max_iter; iter++) {
    scheduler.pause();
    float iter_time = timer.pause();
    float rmse = model->calc_rmse(R);
    logger.put_table_row(iter, 2, iter_time, rmse);
    timer.resume();
    scheduler.resume();
  }
  timer.stop("ends.");
  scheduler.terminate();
}

} // namespace fpsgd
} // namespace parallel
} // namespace cpmf
