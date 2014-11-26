#include <cmath>
#include <random>
#include "common.hpp"

namespace cpmf {
namespace common {

Model::Model(const cpmf::ModelParams &model_params,
              const int &num_u, const int &num_i)
    : params(model_params) {
  P.resize(num_u, std::vector<float> (params.dim) );
  Q.resize(num_i, std::vector<float> (params.dim) );
  initialize(P.data());
  initialize(Q.data());
}

void Model::initialize(std::vector<float> * column) {
  std::random_device rd;
  std::mt19937 mt(rd());
  for (auto elem = column->begin(), i_end = column->end();
      elem != i_end; ++elem) {
    *elem = mt() % 1000 / 1000.0;
  }
}

float Model::calc_rmse(const std::shared_ptr<Matrix> R) {
  double loss = 0.0;
  for (const auto &block : R->blocks) {
    for (const auto &node : block.nodes) {
      float error = calc_error(node.user_id-1, node.item_id-1, node.rating);
      loss += error * error;
    }
  }
  return std::sqrt(loss/R->num_ratings);
}


} // namespace common
} // namespace cpmf
