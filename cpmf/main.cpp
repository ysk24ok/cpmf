#include <iostream>
#include <string>
#include <memory>
#include <cstdlib>

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

#include <cpmf/common/matrix.cpp>
#include <cpmf/common/model.cpp>
#include <cpmf/utils/timer.cpp>
#include <cpmf/utils/logger.cpp>
#include "config.hpp"

#if defined TASK_PARALLEL
#include <cpmf/parallel/task_parallel_based/train.cpp>
#endif

namespace cpmf {

std::shared_ptr<cpmf::Config> parse_config_json(FILE * fp) {
  char buffer[65536];
  rapidjson::FileReadStream is(fp, buffer, sizeof(buffer));
  rapidjson::Document doc;
  doc.ParseStream<0, rapidjson::UTF8<>, rapidjson::FileReadStream>(is);

  std::shared_ptr<cpmf::Config> config(new cpmf::Config);
  config->params.dim       = doc["dimension"].GetInt();
  config->params.step_size = (float) doc["step_size"].GetDouble();
  config->params.lp = (float) doc["regularization_cost_for_P"].GetDouble();
  config->params.lq = (float) doc["regularization_cost_for_Q"].GetDouble();

  config->max_iter        = doc["max_iter"].GetInt();
  config->num_user_blocks = doc["num_user_blocks"].GetInt();
  config->num_item_blocks = doc["num_item_blocks"].GetInt();
  config->input_path      = doc["input_path"].GetString();

  return config;
}


} // namespace cpmf


void main_help() {
  printf(
    "Usage: ./cpmf <command> [<args>]\n"
    "\n"
    "Available commands are listed below:\n"
    "    train     Train a model\n"
  );
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    main_help();
    return EXIT_FAILURE;
  }

  cpmf::utils::Timer timer;

  // parse config.json
  FILE * fp_json = fopen("./config.json", "r");
  if (fp_json == NULL) {
    fprintf(stderr, "Error: Cannot open config.json");
  }
  std::shared_ptr<cpmf::Config> config = cpmf::parse_config_json(fp_json);
  fclose(fp_json);

  // parse input_data
  timer.start("Now parsing input data...");
  FILE * fp_input = fopen(config->input_path.c_str(), "r");
  if (fp_input == NULL) {
    fprintf(stderr, "Error: Cannot open input data");
  }
  std::shared_ptr<cpmf::common::Matrix> R(new cpmf::common::Matrix(
                                          config->num_user_blocks,
                                          config->num_item_blocks, fp_input));
  fclose(fp_input);
  timer.stop("ends.");

  // initialize model
  timer.start("Now initializing model...");
  std::shared_ptr<cpmf::common::Model> model(new cpmf::common::Model(
                                             config->params,
                                             R->num_users, R->num_items));
  timer.stop("ends.");

  // begin training
  cpmf::parallel::task_parallel_based::train(R, model, config->max_iter);

  return EXIT_SUCCESS;
}
