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

Model::~Model() {}


} // namespace common
} // namespace cpmf
