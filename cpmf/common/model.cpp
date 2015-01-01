#include <cmath>
#include <numeric>
#include <random>

#include "common.hpp"

namespace cpmf {
namespace common {

Model::Model(const cpmf::ModelParams &model_params,
             const std::shared_ptr<Matrix> R)
    : params_(model_params), num_users_(R->num_users), num_items_(R->num_items),
      num_blocks_(R->blocks.size()),
      P(new float[num_users_ * params_.dim]),
      Q(new float[num_items_ * params_.dim]) {
  fill_with_random_value(P, num_users_ * params_.dim);
  fill_with_random_value(Q, num_items_ * params_.dim);
  set_initial_losses(R->blocks);
}

float Model::calc_rmse() {
  float sum = 0;
  long num_ratings = 0;
  for (const auto &x : losses_) {
    sum += std::accumulate(x.begin(), x.end(), 0.0);
    num_ratings += x.size();
  }
  return std::sqrt(sum/num_ratings);
}

void Model::fill_with_random_value(std::unique_ptr<float> &uniq_p,
                                   const int &size) {
  std::random_device rd;
  std::mt19937 mt(rd());
  float * raw_p = uniq_p.get();
  for (int i = 0; i < size; i++) { raw_p[i] = mt() % 1000 / 1000.0; }
}

void Model::set_initial_losses(const std::vector<Block> &blocks) {
  const int dim = params_.dim;
  losses_.resize(num_blocks_, std::vector<float> (0.0));
  for (int bid = 0; bid < num_blocks_; bid++) {
    const int num_nodes = blocks[bid].nodes.size();
    losses_[bid].resize(num_nodes, 0.0);
    for (int nid = 0; nid < num_nodes; nid++) {
      const Node &node = blocks[bid].nodes[nid];
      // TODO: duplicate calculation
      float * p = P.get() + (node.user_id - 1) * dim;
      float * q = Q.get() + (node.item_id - 1) * dim;
      float error = node.rating - std::inner_product(p, p+dim, q, 0.0);
      losses_[bid][nid] = error * error;
    }
  }
}

} // namespace common
} // namespace cpmf
