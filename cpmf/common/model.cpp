#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <random>
#include <sstream>
#include <string>

#include "common.hpp"

namespace cpmf {
namespace common {

Model::Model(const cpmf::ModelParams &model_params,
             const std::shared_ptr<Matrix> R)
    : params_(model_params), num_users_(R->num_users), num_items_(R->num_items),
      num_blocks_(R->blocks.size()),
      P(new float[num_users_ * params_.dim]),
      Q(new float[num_items_ * params_.dim]) {
  if (params_.read_model) {
    read_from_disk();
  } else {
    fill_with_random_value(P, num_users_ * params_.dim);
    fill_with_random_value(Q, num_items_ * params_.dim);
  }
}

float Model::calc_rmse(const std::vector<Node> &nodes) {
  float sum = 0.0;
  for (const auto &node: nodes) {
    float error = calc_error(node);
    sum += error * error;
  }
  return std::sqrt(sum / nodes.size());
}

void Model::fill_with_random_value(std::unique_ptr<float> &uniq_p,
                                   const int &size) {
  std::random_device rd;
  std::mt19937 mt(rd());
  float * raw_p = uniq_p.get();
  for (int i = 0; i < size; i++) { raw_p[i] = mt() % 1000 / 1000.0; }
}

void Model::read_from_disk() {
  int dim, num_u, num_i, num_blks;
  std::ifstream fin(params_.output_path, std::ios::in | std::ios::binary);
  // TODO: exception has to be thrown
  if (fin.fail()) {
    std::cerr << "FileReadError: Cannot open " << params_.output_path
      << std::endl;
  }
  fin.read(reinterpret_cast<char*>(&dim), sizeof(dim));
  fin.read(reinterpret_cast<char*>(&num_u), sizeof(num_users_));
  fin.read(reinterpret_cast<char*>(&num_i), sizeof(num_items_));
  fin.read(reinterpret_cast<char*>(&num_blks), sizeof(num_blocks_));

  // check wheather meta data of binary file and Model::params_ are equal
  assert(dim == params_.dim);
  assert(num_users_ == num_u);
  assert(num_items_ == num_i);
  assert(num_blocks_ == num_blks);

  float * p = P.get();
  float * q = Q.get();
  for (int i = 0; i < num_u * params_.dim; i++) {
    fin.read(reinterpret_cast<char*>(&p[i]), sizeof(float));
  }
  for (int i = 0; i < num_i * params_.dim; i++) {
    fin.read(reinterpret_cast<char*>(&q[i]), sizeof(float));
  }
}

void Model::write_to_disk() {
  std::ofstream fout(params_.output_path, std::ios::out | std::ios::binary);
  // TODO: exception has to be thrown
  if (fout.fail()) {
    std::cerr << "FileReadError: Cannot open " << params_.output_path
      << std::endl;
  }
  float * p = P.get();
  float * q = Q.get();

  fout.write(reinterpret_cast<const char*>(&params_.dim), sizeof(num_users_));
  fout.write(reinterpret_cast<const char*>(&num_users_), sizeof(num_users_));
  fout.write(reinterpret_cast<const char*>(&num_items_), sizeof(num_items_));
  fout.write(reinterpret_cast<const char*>(&num_blocks_), sizeof(num_blocks_));
  for (int i = 0, size = num_users_ * params_.dim; i < size; i++) {
    fout.write(reinterpret_cast<char*>(&p[i]), sizeof(float));
  }
  for (int i = 0, size = num_items_ * params_.dim; i < size; i++) {
    fout.write(reinterpret_cast<char*>(&q[i]), sizeof(float));
  }
}

void Model::show_info(const std::string &message) {
  std::ostringstream oss;
  oss << message << std::endl;
  oss << "  dimension        : " << params_.dim << std::endl;
  oss << "  step size        : " << params_.step_size << std::endl;
  oss << "  regularizer of P : " << params_.lp << std::endl;
  oss << "  regularizer of Q : " << params_.lq << std::endl;
  oss << "  output path      : " << params_.output_path << std::endl;
  std::cout << oss.str() << std::endl;
}

} // namespace common
} // namespace cpmf
