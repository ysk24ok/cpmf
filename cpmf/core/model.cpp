#include <vector>
#include "core.hpp"

namespace cpmf {
namespace core {

Model::Model(cpmf::Parameter &config_params, int const num_u, int const num_i) {
  params = config_params;
  num_users = num_u;
  num_items = num_i;

  initialize(P);
  initialize(Q);
}

void Model::initialize(std::vector<std::vector<float>> &model_matrix) {
  model_matrix.resize(num_users, std::vector<float> (params.dim) );

  for (auto columns : model_matrix) {
    for (auto elem : columns) {
      elem = 0.0;
    }
  }
}

Model::~Model() {}


} // namespace core
} // namespace cpmf
