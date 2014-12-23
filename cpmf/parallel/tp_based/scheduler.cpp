#include "tp_based.hpp"

namespace cpmf {
namespace parallel {
namespace tp_based {

void Scheduler::start(const std::shared_ptr<cpmf::common::Matrix> R,
                      std::shared_ptr<cpmf::common::Model> model) {
  matrix_ptr = R;
  model_ptr = model;
}

void Scheduler::wait_for_all_blocks_processed() {
  mk_task_group;
  // TODO: Here, assume the num_user_blocks and num_item_blocks are equal
  cpmf_spawn( grid_recursively(matrix_ptr->num_user_blocks, 0, 0) );
  cpmf_sync;
}

void Scheduler::grid_recursively(const int &block_length,
                                 const int &initial_user_id,
                                 const int &initial_item_id) {
  const int half_len = block_length / 2;
  if (half_len == 0) {
    const int block_id = initial_user_id * matrix_ptr->num_item_blocks
                          + initial_item_id;
    model_ptr->sgd(matrix_ptr->blocks[block_id]);
    return;
  }

  const int boundary_uid = initial_user_id + half_len;
  const int boundary_iid = initial_item_id + half_len;

  // spawn tasks on the same diagonal line
  mk_task_group;
  cpmf_spawn( grid_recursively(half_len, initial_user_id, initial_item_id) );
  cpmf_spawn( grid_recursively(half_len, boundary_uid, boundary_iid) );
  cpmf_sync;
  cpmf_spawn( grid_recursively(half_len, initial_user_id, boundary_iid) );
  cpmf_spawn( grid_recursively(half_len, boundary_uid, initial_item_id) );
  cpmf_sync;
}

} // namespace tp_based
} // namespace parallel
} // namespace cpmf
