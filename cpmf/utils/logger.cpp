#include <cstdarg>
#include <cstring>
#include <iostream>
#include "utils.hpp"

namespace cpmf {
namespace utils {

void Logger::put_msg(const std::string &msg) {
  if (msg != "") { std::cout << msg << std::endl; }
}

void Logger::put_msg_with_time(const std::string &msg, const float &time_sec) {
  if (msg == "") { return; }
  std::string t = "(" + std::to_string(time_sec) + time_sec_suffix_ + ")";
  std::cout << msg + basic_blank_ + t << std::endl;
}

void Logger::put_table_header(const char * id, const int &num, ...) {
  char header[80] = "|";
  char border[80] = "|";
  sprintf(header+strlen(header), "%10s|", id);
  sprintf(border+strlen(border), "----------|");

  char * str;
  va_list args;
  va_start(args, num);
  for (int i = 0; i < num; i++) {
    str = va_arg(args, char *);
    sprintf(header+strlen(header), "%10s|", str);
    sprintf(border+strlen(border), "----------|");
  }
  va_end(args);
  printf("%s\n%s\n", header, border);
}

void Logger::put_table_row(const int &id, const int &num, ...) {
  char row[80] = "|";
  sprintf(row+strlen(row), "%10d|", id);

  double val;
  va_list args;
  va_start(args, num);
  for (int i = 0; i < num; i++) {
    val = va_arg(args, double);
    sprintf(row+strlen(row), "%10.3lf|", val);
  }
  va_end(args);
  printf("%s\n", row);
}

} // namespace utils
} // namespace cpmf
