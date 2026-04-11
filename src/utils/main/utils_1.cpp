#include <iostream>
#include <fstream>
#ifdef _WIN32
#include "../../../lib-w/json/json.hpp"
    #include <windows.h>
    #define sleep_1 Sleep(300);
#else
#include "../../../lib-l/json/json.hpp"
    #include <unistd.h>
    #define sleep_1 usleep(300 * 1000);
#endif
#include "utils_1.h"
  
using json=nlohmann::json;
using namespace std;
namespace fs = std::filesystem;
#define PRINT_ERROR std::cout<<"Unexpected Error Occurred : 12";
//Miscellaneous tools

string get_home(const char* argument_0){
    fs::path p =fs::absolute(fs::path(argument_0)).parent_path();
    return p.string();
}

string build_path(const std::string& argument_0,const std::string& arg1){
    fs::path p =fs::absolute(fs::path(argument_0));
    return (p / arg1).string();
}

string read_as_string(const string& filename){
    std::string output="";
    fs::path p=fs::path(filename);  
    std::ifstream file(p.string());
    if (file.is_open()) {
        file.seekg(0, std::ios::end);
        int size_ = file.tellg();
        file.seekg(0, std::ios::beg);
        
        output.resize(size_);
        file.read(&output[0], size_);
        file.close();
    } else {
        output="none Description";
        cout<<"Warning:"<<filename<<" is not found\n";
    }
    return output;
}

string get_time(){
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss<< std::put_time(std::localtime(&time_t_now),("%m_%d_%H_%M_"));
    return ss.str();
}

string get_env(int provider){
    char buffer[256];
    switch (provider)
    {
    case 0:
        snprintf(buffer,sizeof(buffer),"%s",std::getenv("DEEPSEEK_API_KEY"));
        break;
    case 1:
        snprintf(buffer,sizeof(buffer),"%s",std::getenv("OPENAI_API_KEY"));
        break;
    case 2:
        snprintf(buffer,sizeof(buffer),"%s",std::getenv("XAI_API_KEY"));
        break;
    case 3:
        snprintf(buffer,sizeof(buffer),"%s",std::getenv("QWEN_API_KEY"));
        break;
    case 4:
        snprintf(buffer,sizeof(buffer),"%s",std::getenv("COHERE_API_KEY"));
        break;
    case 5:
        snprintf(buffer,sizeof(buffer),"%s",std::getenv("OPENROUTE_API_KEY"));
        break;
    default:
        break;
    }
    return buffer;
}
 

