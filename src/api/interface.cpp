
#include <iostream>
#include "sagtapi.h"
#include "../agent/sagent.h"
#include "../local_tool/cli_tool.h"
#include "../utils/main/utils_1.h"
#define PRINT_ERROR std::cout<<"Unexpected Error Occurred : 9";
using namespace std;

void sagtlib::sagent::toggle_debug(){
    this->debugger=!this->debugger;
    cout<<"Debug:"<<(this->debugger?"on":"off")<<endl;
}
sagtlib::sagent* sagtlib::sagent::create(const char* home,const string& room){
    Agent* A=new Agent(build_path(get_home(home),".slcache"),room);
    agent_home=P(P(get_home(home),".slcache"),room);
    {
        DEFINE_TOOL(
            terminal_,
            "To execute a shell command on the local linux/windows machine;\
            default path of execution is in your agent home;\
            this tool is Not Favored,please turn to other tool to achieve task if possible;\
            Mainly used for Reading files;\
            Do not try to Execute a script or binary or write/move/copy files unless user Explicitly permits,and Conversely,it is better to guide user to do it.",
            {"1","command",STRING_PARAMETER,"The shell command to be Executed", ""}
        )
        REGISTER_TOOL(A,terminal_,terminal_tool);//tag 0
    }
    {
        std::function<std::string(const std::string*)> func1 = std::bind(&sagtlib::Agent::get_skills, A, std::placeholders::_1);
        DEFINE_TOOL(
            help_,
            "to see what Dynamic-load(DL) tools are available to be activated",
            {"1","show",INT_PARAMETER,"use 1 to show all DL tool with tags on", "1"}
        )
        REGISTER_TOOL(A,help_,func1);//tag 1
    }
    {
        std::function<std::string(const std::string*)> func2 = std::bind(&sagtlib::Agent::activate_tool, A, std::placeholders::_1);
        DEFINE_TOOL(
            select_,
            "To select dynamic-load(DL) tool to activate by tag for a short period, DL tool will be disabled again once you Eventually reply user without tool calling",
            {"1","tool_tag",INT_PARAMETER,"the tag of which the DL tool to be activated", ""}
        )
        REGISTER_TOOL(A,select_,func2);//tag 2
    }
  
    cout<<A->load_ex();

    return A;
}
void sagtlib::sagent::destroy(sagtlib::sagent* instance){
    delete instance;
}
sagtlib::sagent::~sagent() {}
 