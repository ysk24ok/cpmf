#include <vector>
#include <memory>

#include <cpmf/common/common.hpp>
#include <cpmf/utils/utils.hpp>
#include "tp_switch.hpp"

namespace cpmf {
namespace parallel {
namespace task_parallel_based {

void divide(const std::shared_ptr<cpmf::common::Matrix> R,
            std::shared_ptr<cpmf::common::Model> model,
            const int &block_length,
            const int &initial_user_id, const int &initial_item_id) {
  const int half_len = block_length / 2;
  if (half_len == 0) {
    const int block_id = initial_user_id * R->num_item_blocks + initial_item_id;
    model->sgd(R->blocks[block_id]);
    return;
  }

  const int boundary_uid = initial_user_id + half_len;
  const int boundary_iid = initial_item_id + half_len;

  // spawn tasks on the same diagonal line
  mk_task_group;
  cpmf_spawn( divide(R, model, half_len, initial_user_id, initial_item_id) );
  cpmf_spawn( divide(R, model, half_len, boundary_uid, boundary_iid) );
  cpmf_sync;
  cpmf_spawn( divide(R, model, half_len, initial_user_id, boundary_iid) );
  cpmf_spawn( divide(R, model, half_len, boundary_uid, initial_item_id) );
  cpmf_sync;
}

void train(const std::shared_ptr<cpmf::common::Matrix> R,
           std::shared_ptr<cpmf::common::Model> model,
           const cpmf::BaseParams &base_params) {
  cpmf::utils::Timer timer;
  cpmf::utils::Logger logger;
  mk_task_group;

  timer.start("Now iteration starts...");
  logger.put_table_header("iteration", 2, "time", "RMSE");
  for (int iter = 1; iter <= base_params.max_iter; iter++) {
    // TODO: Here, assume the num_user_blocks and num_item_blocks are equal
    cpmf_spawn( divide(R, model, R->num_user_blocks, 0, 0) );
    cpmf_sync;
    float iter_time = timer.pause();
    float rmse = model->calc_rmse(R);
    logger.put_table_row(iter, 2, iter_time, rmse);
    timer.resume();
  }
  timer.stop("ends.");
}

} // namespace task_parallel_based
} // namespace parallel
} // namespace cpmf
