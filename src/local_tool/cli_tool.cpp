#include <string>
#include "cli_tool.h"
#include "../utils/file/util_parse_file.h"
using namespace std;

string skill_1(const string* data){
    string cmd=data[0];//
    char buffer[128];
    string result = "";
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "Can't open another pipe right now";
    while (fgets(buffer, sizeof(buffer), pipe) != NULL)result += buffer;
    pclose(pipe);
    return result;
};
string skill_2(const string* data){
    return decode_image(data[0]);
}