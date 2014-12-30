#ifndef CPMF_CONFIG_HPP_
#define CPMF_CONFIG_HPP_

#include <string>

namespace cpmf {

struct BaseParams {
  BaseParams() : max_iter(10), num_threads(4), calc_rmse(true) {};

  int max_iter, num_threads;
  bool calc_rmse;
};

struct ModelParams {
  ModelParams() :
    dim(40), lp(1), lq(1), step_size(0.005),
    input_path("./model.bin"), output_path("./model.bin") {}

  int dim;
  float lp, lq, step_size;
  std::string input_path, output_path;
};

struct DataParams {
  DataParams() :
    num_user_blocks(1), num_item_blocks(1),
    randomize(false),
    input_path("./input/testdata"), output_path("./output/testdata.bin") {}

  int num_user_blocks, num_item_blocks;
  bool randomize;
  std::string input_path, output_path;
};

struct Config {
  Config() :
    base_params(BaseParams()), model_params(ModelParams()),
    data_params(DataParams()) {}

  BaseParams base_params;
  ModelParams model_params;
  DataParams data_params;
};

} // namespace cpmf

#endif
