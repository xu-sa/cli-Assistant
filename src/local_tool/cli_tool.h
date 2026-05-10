#ifndef CLI_TOOL
#define CLI_TOOL
#include <string>
extern std::string terminal_path;
std::string terminal_tool(const std::string* data);
std::string external_tool(const std::string* data);
std::string change_dir(const std::string* dir);
#endif