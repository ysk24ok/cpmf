#include <cpmf/utils/utils.hpp>
#include "tp_based.hpp"

namespace cpmf {
namespace parallel {
namespace tp_based {

void train(const std::shared_ptr<cpmf::common::Matrix> R,
           std::shared_ptr<cpmf::common::Model> model,
           const cpmf::BaseParams &base_params) {
  cpmf::utils::Timer timer;
  cpmf::utils::Logger logger;
  Scheduler scheduler(R->num_user_blocks, R->num_item_blocks,
                      base_params.num_threads);

  timer.start("Now iteration starts...");
  logger.put_table_header("iteration", 2, "time", "RMSE");
  scheduler.start(R, model);
  for (int iter = 1; iter <= base_params.max_iter; iter++) {
    scheduler.wait_for_all_blocks_processed();
    float iter_time = timer.pause();
    float rmse = model->calc_rmse(R);
    logger.put_table_row(iter, 2, iter_time, rmse);
    timer.resume();
    scheduler.resume();
  }
  timer.stop("ends.");
  scheduler.terminate();
}

} // namespace tp_based
} // namespace parallel
} // namespace cpmf
