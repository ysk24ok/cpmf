#include <cpmf/common/common.hpp>
#include <cpmf/utils/utils.hpp>
#include "switch.hpp"

namespace cpmf {
namespace parallel {

void train(const std::shared_ptr<cpmf::common::Matrix> R,
           std::shared_ptr<cpmf::common::Model> model,
           const cpmf::BaseParams &base_params) {
  cpmf::utils::Timer timer;
  cpmf::utils::Logger logger;
  Scheduler scheduler(R->num_user_blocks, R->num_item_blocks,
                      base_params.num_threads);

  // print header
  if (base_params.calc_rmse) {
    logger.put_table_header("iteration", 2, "time", "RMSE");
    logger.put_table_row(0, 2, 0.000, model->calc_rmse());
  } else {
    logger.put_table_header("iteration", 1, "time");
    logger.put_table_row(0, 1, 0.000);
  }

  timer.start();
  scheduler.start(R, model);
  for (int iter = 1; iter <= base_params.max_iter; iter++) {
    scheduler.wait_for_all_blocks_processed();
    float iter_time = timer.pause();
    if (base_params.calc_rmse) {
      logger.put_table_row(iter, 2, iter_time, model->calc_rmse());
    } else {
      logger.put_table_row(iter, 1, iter_time);
    }
    timer.resume();
    scheduler.resume();
  }
  timer.stop("traininig finishes.");
  scheduler.terminate();
}

} // namespace parallel
} // namespace cpmf
