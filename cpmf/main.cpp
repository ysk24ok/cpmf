#include <iostream>
#include <string>
#include <cstdlib>

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

#include <cpmf/core/matrix.cpp>
#include <cpmf/core/model.cpp>
#include "config.hpp"

namespace cpmf {

std::shared_ptr<cpmf::Config> parse_config_json(FILE * fp) {
  char buffer[65536];
  rapidjson::FileReadStream is(fp, buffer, sizeof(buffer));
  rapidjson::Document doc;
  doc.ParseStream<0, rapidjson::UTF8<>, rapidjson::FileReadStream>(is);

  std::shared_ptr<cpmf::Config> config_ptr(new cpmf::Config);
  config_ptr->params.dim       = doc["dimension"].GetInt();
  config_ptr->params.step_size = (float) doc["step_size"].GetDouble();
  config_ptr->params.lp = (float) doc["regularization_cost_for_P"].GetDouble();
  config_ptr->params.lq = (float) doc["regularization_cost_for_Q"].GetDouble();

  config_ptr->max_iter        = doc["max_iter"].GetInt();
  config_ptr->num_user_blocks = doc["num_user_blocks"].GetInt();
  config_ptr->num_item_blocks = doc["num_item_blocks"].GetInt();
  config_ptr->parallel_method = doc["parallel_method"].GetString();
  config_ptr->input_path      = doc["input_path"].GetString();

  return config_ptr;
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

  // parse config.json
  FILE * fp_json = fopen("./config.json", "r");
  if (fp_json == NULL) {
    fprintf(stderr, "Error: Cannot open config.json");
  }
  std::shared_ptr<cpmf::Config> config_ptr = cpmf::parse_config_json(fp_json);
  fclose(fp_json);

  // parse input_data
  FILE * fp_input = fopen(config_ptr->input_path.c_str(), "r");
  if (fp_input == NULL) {
    fprintf(stderr, "Error: Cannot open input data");
  }
  std::shared_ptr<cpmf::core::Matrix> R(new cpmf::core::Matrix(config_ptr->num_user_blocks, config_ptr->num_item_blocks, fp_input));
  fclose(fp_input);

  // initialize model
  std::shared_ptr<cpmf::core::Model> model(new cpmf::core::Model(config_ptr->params, R->num_users, R->num_items));

  return EXIT_SUCCESS;
}
