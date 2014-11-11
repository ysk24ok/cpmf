#include <cmath>
#include <random>
#include "common.hpp"

namespace cpmf {
namespace common {

Model::Model(cpmf::Parameter &config_params, int const num_u, int const num_i)
  : params(config_params), num_users(num_u), num_items(num_i) {
  P.resize(num_users, std::vector<float> (params.dim) );
  Q.resize(num_items, std::vector<float> (params.dim) );
  initialize(P);
  initialize(Q);
}

void Model::initialize(std::vector<std::vector<float>> &model_matrix) {
  std::random_device rd;
  std::mt19937 mt(rd());
  for (auto &columns : model_matrix) {
    for (float &elem : columns) {
      elem = mt() % 1000 / 1000.0;
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
