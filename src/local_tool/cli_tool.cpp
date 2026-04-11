//#include <string>
#include "cli_tool.h"
#include "string.h"
#include "../utils/main/utils_1.h"
#ifdef _WIN32
    #define POPEN _popen
    #define PCLOSE _pclose
#else
    #define POPEN popen
    #define PCLOSE pclose
#endif
#define PRINT_ERROR std::cout<<"Unexpected Error Occurred : 10";
#define MAX_TOOL_OUTPUT 1024*3
static char buffer[1024]={0};
using namespace std;

string skill_1(const string* data) {//1
    string result = "";
    string cmd = data[0];
    #ifndef _WIN32
    cmd = "(" + cmd + ") 2>&1";
    #endif
    FILE* pipe = POPEN(cmd.c_str(), "r");
    if (!pipe) return "Error: Failed to open pipe.";
    size_t total_read = 0;
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        result += buffer;
        total_read += strlen(buffer);
        if (total_read > MAX_TOOL_OUTPUT) { 
            result += "\nWarning: Stopping reading from tool due to length limits...";
            break; 
        }
    }
    int exit_code = PCLOSE(pipe);
    if (exit_code != 0) {
        result += "\n[Process exited with code: " + to_string(exit_code) + "]";
    }
    return result.empty() ? "Command executed with no output" : result;
};

string skill_2(const string* data) {//2
    string python_path=build_path(build_path(build_path(extension_home,"venv"),"bin"),"python3 ");
    string cmd=python_path+\
        build_path(build_path(extension_home,data[0]),data[0]+".py ")+\
        "'"+data[1]+"'"+" 2>&1";
    return skill_1(&cmd);
}