#ifndef CPMF_CORE_HPP_
#define CPMF_CORE_HPP_

#include <memory>
#include <numeric>
#include <vector>
#include <cpmf/config.hpp>

namespace cpmf {
namespace common {

struct Node {
  Node() : user_id(0), item_id(0), rating(0) {}
  int user_id, item_id;
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
  Matrix(const int &num_u_blks, const int &num_i_blks, FILE * fp_input);
  ~Matrix();

  int num_users, num_items, num_user_blocks, num_item_blocks;
  long num_ratings;
  std::vector<Block> blocks;

 private:
  void initialize_blocks();
  void read(FILE * fp_input, std::vector<Node> * temp_nodes);
  void assign_nodes(const std::vector<Node> &temp_nodes);
  void sort_nodes_by_user_id();
};


class Model {
 public:
  Model(const cpmf::Parameter &conf_params, const int &num_u, const int &num_i);
  ~Model();

  float calc_rmse(const std::shared_ptr<Matrix> R);
  inline void sgd(const Block &block);

  Parameter params;
  int num_users, num_items;
  std::vector<std::vector<float>> P, Q;

 private:
  void initialize(std::vector<float> * column);
  inline float calc_error(const int &uid, const int &iid, const int &rate);
};

inline float Model::calc_error(const int &uid, const int &iid,
                                const int &rate) {
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

#endif // CPMF_CORE_HPP_
