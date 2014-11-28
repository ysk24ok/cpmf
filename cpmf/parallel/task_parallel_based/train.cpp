#include <vector>
#include <memory>

#include <cpmf/common/common.hpp>
#include <cpmf/utils/utils.hpp>
#include "tp_switch.hpp"

namespace cpmf {
namespace parallel {
namespace task_parallel_based {

void divide(std::shared_ptr<cpmf::common::Matrix> const R,
            std::shared_ptr<cpmf::common::Model> model,
            int const block_length,
            int const initial_user_id, int const initial_item_id) {
  int const half_len = block_length / 2;
  if (half_len == 0) {
    int const block_id = initial_user_id * R->num_user_blocks + initial_item_id;
    model->sgd(R->blocks[block_id]);
    return;
  }

  int const boundary_uid = initial_user_id + half_len;
  int const boundary_iid = initial_item_id + half_len;

  // spawn tasks on the same diagonal line
  mk_task_group;
  cpmf_spawn( divide(R, model, half_len, initial_user_id, initial_item_id) );
  cpmf_spawn( divide(R, model, half_len, boundary_uid, boundary_iid) );
  cpmf_sync;
  cpmf_spawn( divide(R, model, half_len, initial_user_id, boundary_iid) );
  cpmf_spawn( divide(R, model, half_len, boundary_uid, initial_item_id) );
  cpmf_sync;
}

void train(std::shared_ptr<cpmf::common::Matrix> const R,
           std::shared_ptr<cpmf::common::Model> model,
           int const max_iter) {
  cpmf::utils::Timer timer;
  cpmf::utils::Logger logger;
  mk_task_group;

  timer.start("Now iteration starts...");
  logger.put_table_header("iteration", 2, "time", "RMSE");
  for (int iter = 1; iter <= max_iter; iter++) {
    timer.resume();
    // TODO: Here, assume the num_user_blocks and num_item_blocks are equal
    cpmf_spawn( divide(R, model, R->num_user_blocks, 0, 0) );
    cpmf_sync;
    float iter_time = timer.pause();
    float rmse = model->calc_rmse(R);
    logger.put_table_row(iter, 2, iter_time, rmse);
  }
  timer.stop("ends.");
}

} // namespace task_based
} // namespace train
} // namespace cpmf
