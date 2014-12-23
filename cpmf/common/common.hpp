#ifndef CPMF_COMMON_HPP_
#define CPMF_COMMON_HPP_

#include <memory>
#include <numeric>
#include <string>
#include <vector>

#include <cpmf/config.hpp>

namespace cpmf {
namespace common {

struct Node {
  Node()
    : user_id(0), item_id(0), orig_user_id(0), orig_item_id(0), rating(0.0) {}
  int user_id, item_id, orig_user_id, orig_item_id;
  float rating;
};

struct Block {
  Block(const int &block_user_id, const int &block_item_id)
    : user_id(block_user_id), item_id(block_item_id), nodes(0) {}
  int user_id, item_id;
  std::vector<Node> nodes;
};

class Matrix {
 public:
  Matrix(const cpmf::DataParams &data_params);
  void show_info();

  long num_ratings;
  int num_users, num_items, num_user_blocks, num_item_blocks;
  std::vector<Block> blocks;

 private:
  void initialize_blocks();
  void read(std::vector<Node> * temp_nodes);
  void generate_mapping_vector(std::vector<int> * mapping_vec, bool randomize);
  void assign_nodes(std::vector<Node> * temp_nodes,
                    const std::vector<int> &user_mapping,
                    const std::vector<int> &item_mapping);
  void sort_nodes_by_user_id();

  std::string input_path_;
  std::string output_path_;
};


class Model {
 public:
  Model(const cpmf::ModelParams &model_params,
        const int &num_u, const int &num_i);

  float calc_rmse(const std::shared_ptr<Matrix> R);
  inline void sgd(const Block &block);

  cpmf::ModelParams params;
  std::vector<std::vector<float>> P, Q;

 private:
  void initialize(std::vector<std::vector<float>> * submatrix);
  inline float calc_error(const int &uid, const int &iid, const float &rate);
};

inline float Model::calc_error(const int &uid, const int &iid,
                               const float &rate) {
  const std::vector<float> * p = &P[uid];
  const std::vector<float> * q = &Q[iid];
  return rate - std::inner_product(p->begin(), p->end(), q->begin(), 0.0);
}

inline void Model::sgd(const Block &block) {
  const float step_size = params.step_size;
  const float lp = step_size * params.lp;
  const float lq = step_size * params.lq;

  for (const auto &node : block.nodes) {
    int uid = node.user_id - 1;
    int iid = node.item_id - 1;
    float error = calc_error(uid, iid, node.rating) * step_size;
    for (int d = 0, dim = params.dim; d < dim; d++) {
      float tmp_p_val = P[uid][d];
      P[uid][d] += error * Q[iid][d] - lp * P[uid][d];
      Q[iid][d] += error * tmp_p_val - lq * Q[iid][d];
    }
  }
}

} // namespace common
} // namespace cpmf

#endif // CPMF_COMMON_HPP_
