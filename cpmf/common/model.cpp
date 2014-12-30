#include <cmath>
#include <numeric>
#include <random>

#include "common.hpp"

namespace cpmf {
namespace common {

Model::Model(const cpmf::ModelParams &model_params,
             const std::shared_ptr<Matrix> R)
    : params(model_params) {
  initialize_matrix(P, R->num_users);
  initialize_matrix(Q, R->num_items);
  calc_initial_loss(R->blocks);
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

void Model::initialize_matrix(std::unique_ptr<float> &uniq_p, const int &num) {
  const int alignment = 32;
  const int size = num * params.dim;
  posix_memalign(reinterpret_cast<void**>(&uniq_p), alignment,
                 size * sizeof(float));

  std::random_device rd;
  std::mt19937 mt(rd());
  float * raw_p = uniq_p.get();
  for (int i = 0; i < size; i++) { raw_p[i] = mt() % 1000 / 1000.0; }
}

void Model::calc_initial_loss(std::vector<Block> blocks) {
  const int dim = params.dim;
  const int num_blocks = blocks.size();
  losses_.resize(num_blocks, std::vector<float> (0.0));
  for (int bid = 0; bid < num_blocks; bid++) {
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
