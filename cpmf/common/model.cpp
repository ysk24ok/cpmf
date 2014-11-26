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
  initialize(&P);
  initialize(&Q);
}

void Model::initialize(std::vector<std::vector<float>> * submatrix) {
  std::random_device rd;
  std::mt19937 mt(rd());
  for (int i = 0; i < static_cast<int>(submatrix->size()); i++) {
    std::vector<float> * column = &(*submatrix)[i];
    for (auto elem = (*column).begin(), elem_end = (*column).end();
         elem != elem_end; ++elem) {
      *elem = mt() % 1000 / 1000.0;
    }
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
