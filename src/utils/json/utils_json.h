#ifndef UTILS_1
#define UTILS_1
#include <string>
#include "../../json/json.hpp"
std::string read_as_string(const std::string& home,const char* filepath);
std::string read_as_string(const std::string& home,const std::string& room,const char* filepath);
nlohmann::json handle_read_json(const std::string& home,const std::string& room,const std::string& name);//read local config and chat history
nlohmann::json handle_registration(const std::string definition_[][5],size_t,std::vector<std::string>*);//register tool
std::string handle_get_history(const std::string& home, const std::string& room,int choice);
std::string handle_save(nlohmann::json* save, const std::string& home,const std::string& room,const std::string& filename);//save chat or config to json file
//miscellaneous tools
std::string get_env(int provider);
std::string get_time();
std::string get_home(const char* argument_0);//build a home path
// int set_int(const std::string& str);
// float set_float(const std::string& str);

#endif