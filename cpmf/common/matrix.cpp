#include <vector>
#include <algorithm>
#include "common.hpp"

namespace cpmf {
namespace common {

Matrix::Matrix(int const num_u_blks, int const num_i_blks, FILE * fp_input) {
  initialize(num_u_blks, num_i_blks);
  read(fp_input);
  sort_nodes_by_user_id();
}

void Matrix::initialize(int const num_u_blks, int const num_i_blks) {
  num_users = 0;
  num_items = 0;
  num_user_blocks = num_u_blks;
  num_item_blocks = num_i_blks;

  for (int block_id = 0; block_id < num_user_blocks * num_item_blocks; block_id++) {
    int block_user_id = block_id / num_user_blocks;
    int block_item_id = block_id % num_user_blocks;
    blocks.push_back(Block(block_user_id, block_item_id));
  }
}

void Matrix::read(FILE * fp_input) {
  std::vector<Node> temp_nodes;
  while (true) {
    Node node;
    if (fscanf(fp_input, "%d %d %d\n",
          &node.user_id, &node.item_id, &node.rating) == EOF) {
      break;
    }
    if (node.user_id > num_users) num_users = node.user_id;
    if (node.item_id > num_items) num_items = node.item_id;
    num_ratings++;
    temp_nodes.push_back(node);
  }

  int const blk_u_len = num_users / num_user_blocks;
  int const blk_i_len = num_items / num_item_blocks;

  for (auto &node : temp_nodes) {
    int blk_u_id = (node.user_id - 1) / blk_u_len;
    int blk_i_id = (node.item_id - 1) / blk_i_len;
    int blk_id = blk_u_id * num_item_blocks + blk_i_id;
    blocks[blk_id].nodes.push_back(node);
  }
}


void Matrix::sort_nodes_by_user_id() {
  for (auto block = blocks.begin(); block != blocks.end(); block++) {
    std::sort(block->nodes.begin(), block->nodes.end(),
                [] (const Node& left, const Node& right)
                  -> bool { return left.user_id < right.user_id; } );
  }
}

Matrix::~Matrix() {}


} // namespace common
} // namespace cpmf
