#ifndef UTILS_1
#define UTILS_1
#include <string>


// std::string read_as_string(const std::string& home,const char* filepath);
// std::string read_as_string(const std::string& home,const std::string& room,const char* filepath);
//nlohmann::json handle_read_json(const std::string& home,const std::string& room,const std::string& name);//read local config and chat history
//std::string handle_get_history(const std::string& home, const std::string& room,int choice);
//std::string check_extension_system(const std::string& path);
//std::string handle_save(nlohmann::json* save, const std::string& home,const std::string& room,const std::string& filename);//save chat or config to json file
//miscellaneous tools
std::string get_env(int provider);
std::string build_path(const std::string&  ,const std::string&  );
std::string get_time();
std::string get_home(const char*);
std::string read_as_string(const std::string& filename);
#endif