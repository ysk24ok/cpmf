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
  int user_id, item_id, rating;
};

struct Block {
  Block(int const block_user_id, int const block_item_id)
    : user_id(block_user_id), item_id(block_item_id), nodes(0) {}
  int user_id, item_id;
  std::vector<Node> nodes;
};

class Matrix {
  public:
    int num_users, num_items, num_user_blocks, num_item_blocks;
    long num_ratings;
    std::vector<Block> blocks;

    Matrix(int const num_u_blks, int const num_i_blks, FILE * fp_input);
    ~Matrix();
  private:
    void initialize_blocks();
    void read(FILE * fp_input, std::vector<Node> &temp_nodes);
    void assign_nodes(std::vector<Node> &temp_nodes);
    void sort_nodes_by_user_id();
};


class Model {
  public:
    Parameter params;
    int num_users, num_items;
    std::vector<std::vector<float>> P, Q;

    Model(cpmf::Parameter &config_params, int const num_u, int const num_i);
    float calc_rmse(std::shared_ptr<Matrix> const R);
    inline void sgd(Block const &block);
    ~Model();
  private:
    void initialize(std::vector<std::vector<float>> &model_matrix);
    inline float calc_error(Node const &node);
};

inline float Model::calc_error(Node const &node) {
  std::vector<float> p = P[node.user_id - 1];
  std::vector<float> q = Q[node.item_id - 1];
  return node.rating - std::inner_product(p.begin(), p.end(), q.begin(), 0.0);
}

inline void Model::sgd(Block const &block) {
  for (auto node = block.nodes.begin(); node != block.nodes.end(); node++) {
    float error = calc_error(*node);

    int uid = node->user_id - 1;
    int iid = node->item_id - 1;
    for (int d = 0; d < params.dim; d++) {
      float tmp_p_val = P[uid][d];
      P[uid][d] += params.step_size * (error * Q[iid][d] - params.lp * P[uid][d]);
      Q[iid][d] += params.step_size * (error * tmp_p_val - params.lq * Q[iid][d]);
    }
  }
}

} // namespace common
} // namespace cpmf

#endif // CPMF_CORE_HPP_
