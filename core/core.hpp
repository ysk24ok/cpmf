#ifndef CPMF_CORE_HPP_
#define CPMF_CORE_HPP_

#include <vector>
#include "../config.hpp"

namespace cpmf {
namespace core {

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

} // namespace core
} // namespace cpmf

#endif // CPMF_CORE_HPP_
