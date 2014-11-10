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
    void initialize(int const num_u_blks, int const num_i_blks);
    void read(FILE * fp_input);
    void sort_nodes_by_user_id();
};

class Model {
  public:
    Parameter params;
    int num_users, num_items;
    std::vector<std::vector<float>> P, Q;

    Model(cpmf::Parameter &config_params, int const num_u, int const num_i);
    ~Model();
  private:
    void initialize();
};

inline void sgd(Block const &block, std::shared_ptr<Model> model) {
  int const dim         = model->params.dim;
  float const step_size = model->params.step_size;
  float const lp        = model->params.lp;
  float const lq        = model->params.lq;

  for (auto node = block.nodes.begin(); node != block.nodes.end(); node++) {
    std::vector<float> p = model->P[node->user_id - 1];
    std::vector<float> q = model->Q[node->item_id - 1];

    float error = node->rating - std::inner_product(p.begin(), p.end(), q.begin(), 0.0);

    for (int d = 0; d < dim; d++) {
      float tmp_p = p[d];
      p[d] += step_size * (error * q[d] - lp * p[d]);
      q[d] += step_size * (error * tmp_p - lq * q[d]);
    }
  }
}

} // namespace common
} // namespace cpmf

#endif // CPMF_CORE_HPP_
