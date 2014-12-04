#include <algorithm>
#include <iostream>
#include <fstream>
#include <limits>
#include "common.hpp"

namespace cpmf {
namespace common {

Matrix::Matrix(const cpmf::DataParams &data_params)
    : num_users(0), num_items(0), num_ratings(0),
      num_user_blocks(data_params.num_user_blocks),
      num_item_blocks(data_params.num_item_blocks),
      input_path_(data_params.input_path),
      output_path_(data_params.output_path) {
  initialize_blocks();

  std::vector<Node> temp_nodes;
  read(&temp_nodes);
  assign_nodes(temp_nodes);

  sort_nodes_by_user_id();
}

void Matrix::initialize_blocks() {
  const int total_num_blocks = num_user_blocks * num_item_blocks;
  for (int block_id = 0; block_id < total_num_blocks; block_id++) {
    int block_user_id = block_id / num_user_blocks;
    int block_item_id = block_id % num_user_blocks;
    blocks.push_back(Block(block_user_id, block_item_id));
  }
}

void Matrix::read(std::vector<Node> * temp_nodes) {
  std::ifstream input_ifs(input_path_.c_str());
  if (input_ifs.fail()) {
    std::cerr << "FileReadError: Cannot open " << input_path_ << std::endl;
  }

  std::string line_buf;
  while (getline(input_ifs, line_buf)) {
    Node node;
    sscanf(line_buf.data(),
           "%d %d %f\n", &node.user_id, &node.item_id, &node.rating);
    if (node.user_id > num_users) num_users = node.user_id;
    if (node.item_id > num_items) num_items = node.item_id;
    ++num_ratings;
    temp_nodes->push_back(node);
  }
}

void Matrix::assign_nodes(const std::vector<Node> &temp_nodes) {
  const int blk_u_len = (int) ceil((float) num_users / num_user_blocks);
  const int blk_i_len = (int) ceil((float) num_items / num_item_blocks);

  for (const auto &node : temp_nodes) {
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

void Matrix::show_info() {
  int num_min_ratings = std::numeric_limits<int>::max();
  int num_max_ratings = 0;
  for (int bid = 0, num_blks = blocks.size(); bid < num_blks; bid++) {
    int n = blocks[bid].nodes.size();
    if (n < num_min_ratings) { num_min_ratings = n; }
    if (n > num_max_ratings) { num_max_ratings = n; }
  }

  std::string info = "";
  info+="number of users        : "+std::to_string(num_users)+"\n";
  info+="number of items        : "+std::to_string(num_items)+"\n";
  info+="number of ratings      : "+std::to_string(num_ratings)+"\n";
  info+="number of user blocks  : "+std::to_string(num_user_blocks) + "\n";
  info+="number of item blocks  : "+std::to_string(num_item_blocks) + "\n";
  info+="min ratings in a block : "+std::to_string(num_min_ratings) + "\n";
  info+="max ratings in a block : "+std::to_string(num_max_ratings) + "\n";
  std::cout << info << std::endl;
}


} // namespace common
} // namespace cpmf
