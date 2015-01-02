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
  cpmf_spawn( grid_recursively(matrix_ptr->num_user_blocks,
                               matrix_ptr->num_item_blocks, 0, 0) );
  cpmf_sync;
}

void Scheduler::grid_recursively(const int &user_block_length,
                                 const int &item_block_length,
                                 const int &top_user_id,
                                 const int &left_item_id) {
  const int top_u_blk_len = user_block_length / 2;
  const int left_i_blk_len = item_block_length / 2;
  if (top_u_blk_len == 0 && left_i_blk_len == 0) {
    const int bid = top_user_id * matrix_ptr->num_item_blocks + left_item_id;
    model_ptr->sgd(bid, matrix_ptr->blocks[bid]);
    return;
  }

  const int bottom_user_id = top_user_id + top_u_blk_len;
  const int right_item_id = left_item_id + left_i_blk_len;
  const int bottom_u_blk_len = user_block_length - top_u_blk_len;
  const int right_i_blk_len = item_block_length - left_i_blk_len;

  // spawn tasks on the same diagonal line
  mk_task_group;
  if (top_u_blk_len == 0) {   // 1user * 2items block
    cpmf_spawn( grid_recursively(1, left_i_blk_len,
                                 top_user_id, left_item_id) );
    cpmf_sync;
    cpmf_spawn( grid_recursively(1, right_i_blk_len,
                                 top_user_id, right_item_id) );
    cpmf_sync;

  } else if (left_i_blk_len == 0) {  // 2users * 1item block
    cpmf_spawn( grid_recursively(top_u_blk_len, 1,
                                 top_user_id, left_item_id) );
    cpmf_sync;
    cpmf_spawn( grid_recursively(bottom_u_blk_len, 1,
                                 bottom_user_id, left_item_id) );
    cpmf_sync;

  } else {
    cpmf_spawn( grid_recursively(top_u_blk_len, left_i_blk_len,
                                 top_user_id, left_item_id) );
    cpmf_spawn( grid_recursively(bottom_u_blk_len, right_i_blk_len,
                                 bottom_user_id, right_item_id) );
    cpmf_sync;
    cpmf_spawn( grid_recursively(top_u_blk_len, right_i_blk_len,
                                 top_user_id, right_item_id) );
    cpmf_spawn( grid_recursively(bottom_u_blk_len, left_i_blk_len,
                                 bottom_user_id, left_item_id) );
    cpmf_sync;
  }
}

} // namespace tp_based
} // namespace parallel
} // namespace cpmf
