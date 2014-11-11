#include <cmath>
#include "common.hpp"

namespace cpmf {
namespace common {

Model::Model(cpmf::Parameter &config_params, int const num_u, int const num_i)
  : params(config_params), num_users(num_u), num_items(num_i) {
  P.resize(num_users, std::vector<float> (params.dim) );
  Q.resize(num_items, std::vector<float> (params.dim) );

  initialize();
}

void Model::initialize() {
  for (auto model_matrix : {P, Q}) {
    for (auto columns : model_matrix) {
      for (auto elem : columns) {
        elem = 0.0;
      }
    }
  }
}

float Model::calc_rmse(std::shared_ptr<Matrix> R) {
  double loss = 0;

  for (Block block : R->blocks) {
    for (Node node : block.nodes) {
      float error = calc_error(node);
      loss += error * error;
    }
  }

  return std::sqrt(loss/R->num_ratings);
}

Model::~Model() {}


} // namespace common
} // namespace cpmf
