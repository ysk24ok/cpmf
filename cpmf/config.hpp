#ifndef CPMF_CONFIG_HPP_
#define CPMF_CONFIG_HPP_

#include <string>

namespace cpmf {

struct Parameter {
  Parameter() : dim(40), lp(1), lq(1), step_size(0.005) {}

  int dim;
  float lp, lq, step_size;
};

struct Config {
  Config() :
    max_iter(10), num_user_blocks(1), num_item_blocks(1),
    parallel_method("task based"), input_path("./input/testdata"),
    params() {}

  int max_iter, num_user_blocks, num_item_blocks;
  std::string parallel_method, input_path;
  Parameter params;
};

} // namespace cpmf

#endif
