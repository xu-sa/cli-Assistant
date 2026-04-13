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
 
#define P(x,y) build_path(x,y)
 
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

string skill_2(const string* data) {
    // data[0]: bin,py,sh
    // data[1]: skill name
    // data[2]: skill json parameter
    // data[3]: folder
    string filepath= P(P(data[3],data[1]),data[1]);
    string cmd;
    if (data[0] == "py")cmd =P(P(P(data[3],"venv"),"bin"),"python3 ") + filepath + ".py '" + data[2] + "'";
    else if (data[0] == "sh")cmd = "bash " +filepath+ ".sh '" + data[2] + "'";
    else if (data[0] == "bin")cmd = "./"+filepath + " '" + data[2] + "'";
    else return "Error code: 10";
    return skill_1(&cmd);
}