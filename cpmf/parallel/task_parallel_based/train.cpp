#include <vector>
#include <memory>
#include <cilk/cilk.h>

#include <cpmf/common/common.hpp>
#include <cpmf/utils/utils.hpp>

namespace cpmf {
namespace parallel {
namespace task_parallel_based {

void divide(std::shared_ptr<cpmf::common::Matrix> const R,
            std::shared_ptr<cpmf::common::Model> model,
            int const block_length,
            int const initial_user_id, int const initial_item_id) {
  int const half_block_length = block_length / 2;
  if (half_block_length == 0) {
    int const block_id = initial_user_id * R->num_user_blocks + initial_item_id;
    cpmf::common::sgd(R->blocks[block_id], model);
    return;
  }

  int const boundary_user_id = initial_user_id + half_block_length;
  int const boundary_item_id = initial_item_id + half_block_length;

  // spawn tasks on the same diagonal line
  cilk_spawn divide(R, model, half_block_length, initial_user_id, initial_item_id);
  cilk_spawn divide(R, model, half_block_length, boundary_user_id, boundary_item_id);
  cilk_sync;

  cilk_spawn divide(R, model, half_block_length, initial_user_id, boundary_item_id);
  cilk_spawn divide(R, model, half_block_length, boundary_user_id, initial_item_id);
  cilk_sync;
}

void train(std::shared_ptr<cpmf::common::Matrix> const R,
           std::shared_ptr<cpmf::common::Model> model,
           int const max_iter) {
  cpmf::utils::Timer timer;
  timer.start();
  for (int iter = 1; iter <= max_iter; iter++) {
    timer.resume();
    // TODO: Here, assume the num_user_blocks and num_item_blocks are equal
    cilk_spawn divide(R, model, R->num_user_blocks, 0, 0);
    cilk_sync;
    timer.pause();
  }
}

} // namespace task_based
} // namespace train
} // namespace cpmf