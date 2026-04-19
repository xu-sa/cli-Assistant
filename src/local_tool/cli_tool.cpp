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
#define MAX_TOOL_OUTPUT 1024*2
std::string agent_home="";
static char buffer[512]={0};
using namespace std;
 
string terminal_tool(const string* data) {//1
    string result = "";
    string cmd = data[0];
    #ifndef _WIN32
    cmd = "cd "+agent_home+" && (" + cmd + ") 2>&1";
    #else
    cmd = "cd /d "+agent_home+" && " + cmd;
    #endif
    FILE* pipe = POPEN(cmd.c_str(), "r");
    if (!pipe) return "Error: Failed to open pipe.";
    {
        size_t total_read = 0;
        int read_=1;
        while (1)
        {
            read_ = fread(buffer,1,512,pipe);
            if(read_==0)break;
            buffer[read_]='\0';
            total_read+=read_;
            if(total_read>=MAX_TOOL_OUTPUT){
                result+="[Output Reached Size Bound]";
                break;
            }
            else result+=buffer;
        }
    }
    u_int8_t exit_code = PCLOSE(pipe);
    if (exit_code != 0)result += "\n[Process exited with code: " + to_string(exit_code) + "]";
    return result.empty() ? "Command executed with no output" : result;
};

string external_tool(const string* data) {
    // data[0]: bin,py,sh
    // data[1]: skill name
    // data[2]: skill json parameter
    // data[3]: folder
    string filepath= P(P(data[3],data[1]),data[1]);
    string cmd;
    
    if (data[0] == "py")cmd =P(P(P(data[3],"venv"),"bin"),"python3 ") + filepath + ".py '" + data[2] + "'";
    else if (data[0] == "sh")cmd = "bash " +filepath+ ".sh '" + data[2] + "'";
    else if (data[0] == "bin")cmd = "./"+filepath + " \"" + data[2] + "\"";
    else return "Error code: 10";
    return terminal_tool(&cmd);
}

std::string tool_3(const std::string* data){
    
    return "return from tool_3";
};