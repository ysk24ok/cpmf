#include <iostream>
#include <string>
#include <cstdlib>

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

#include "core/matrix.cpp"

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

  std::shared_ptr<Config> parse_config_json(FILE * fp) {
    char buffer[65536];
    rapidjson::FileReadStream is(fp, buffer, sizeof(buffer));
    rapidjson::Document doc;
    doc.ParseStream<0, rapidjson::UTF8<>, rapidjson::FileReadStream>(is);

    std::shared_ptr<Config> config_ptr(new Config);
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

  void main_help() {
    printf(
      "Usage: ./cpmf <command> [<args>]\n"
      "\n"
      "Available commands are listed below:\n"
      "    train     Train a model\n"
    );
  }

} // namespace cpmf


int main(int argc, char *argv[]) {
  if (argc < 2) {
    cpmf::main_help();
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

  return EXIT_SUCCESS;
}
