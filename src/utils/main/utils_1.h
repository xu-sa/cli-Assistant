#ifndef UTILS_1
#define UTILS_1
#include <string>
#define P(x,y) build_path(x,y)
std::string get_env(int provider);
std::string build_path(const std::string&  ,const std::string&  );
std::string get_time();
std::string get_home(const char*);
std::string read_as_string(const std::string& filename);
#endif