#ifndef CPMF_PARALLEL_TP_BASED_HPP_
#define CPMF_PARALLEL_TP_BASED_HPP_

#include <cpmf/common/common.hpp>
#include "tp_switch.hpp"

namespace cpmf {
namespace parallel {
namespace tp_based {

class Scheduler {
 public:
  Scheduler(const int &num_u_blks, const int &num_i_blks,
            const int &num_thrs) {}
  void start(const std::shared_ptr<cpmf::common::Matrix> R,
             std::shared_ptr<cpmf::common::Model> model);
  void wait_for_all_blocks_processed();
  void resume() {}
  void terminate() {}

 private:
  void grid_recursively(const int &user_block_length,
                        const int &item_block_length,
                        const int &top_user_id, const int &left_item_id);

  std::shared_ptr<cpmf::common::Matrix> matrix_ptr;
  std::shared_ptr<cpmf::common::Model> model_ptr;
};

} // namespace tp_based
} // namespace parallel
} // namespace cpmf

#endif // CPMF_PARALLEL_TP_BASED_HPP_
