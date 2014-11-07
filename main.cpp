#include <iostream>
#include <string>
#include <cstdlib>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

namespace cpmf {

  struct Config {
    Config() :
      dim(40), max_iter(10), step_size(0.005),
      parallel_method("task based") {}

    int dim, max_iter;
    float step_size;
    std::string parallel_method, input_path;
  };

  std::shared_ptr<Config> parse_config_json(FILE * fp) {
    char buffer[65536];
    rapidjson::FileReadStream is(fp, buffer, sizeof(buffer));
    rapidjson::Document doc;
    doc.ParseStream<0, rapidjson::UTF8<>, rapidjson::FileReadStream>(is);

    std::shared_ptr<Config> config_ptr(new Config);
    config_ptr->dim             = doc["dimension"].GetInt();
    config_ptr->max_iter        = doc["max_iter"].GetInt();
    config_ptr->step_size       = (float) doc["step_size"].GetDouble();
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

  return EXIT_SUCCESS;
}
