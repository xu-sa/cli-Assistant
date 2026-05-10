
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

sagtlib::sagent* sagtlib::sagent::create(const char* home,const string& name){
    Agent* A=new Agent(P(P(get_home(home),".syagent"),name),name);
    {
        DEFINE_TOOL(
            command,
            "To execute a shell command on local machine.\
            this tool is Not the prioritized if other tool can do the same task.\
            Do not try to Execute or modify files unless user Explicitly permits,and Conversely,it is better to guide user to do it.",
            {"1","command",STRING_PARAMETER,"The shell command to be Executed", ""}
        )
        REGISTER_TOOL(A,command,terminal_tool);//tag 0
    }
    {
        DEFINE_TOOL(
            chdir,
            "to change agent's default terminal Directory, always use this if you want to maintain the path you changed to after using terminal tool",
            {"1","dir",STRING_PARAMETER,"Directory where your terminal will open at",""}
        )
        REGISTER_TOOL(A,chdir,change_dir);//tag 0
    }
    {
        std::function<std::string(const std::string*)> func1 = std::bind(&sagtlib::Agent::get_plugin, A, std::placeholders::_1);
        DEFINE_TOOL(
            plughelp,
            "to see available plug-in tools which arent activated"
            // {"1","show",INT_PARAMETER,""}
        )
        REGISTER_TOOL(A,plughelp,func1);//tag 1
    }
    {
        std::function<std::string(const std::string*)> func2 = std::bind(&sagtlib::Agent::activate_tool, A, std::placeholders::_1);
        DEFINE_TOOL(
            plugselect,
            "To select plug-in tool to activate using it's tag for a short period, which will be disabled again once you completed replying or tasking",
            {"1","tag",INT_PARAMETER,"the tag of which the plug-in tool to be activated", ""}
        )
        REGISTER_TOOL(A,plugselect,func2);//tag 2
    }
  
    cout<<A->load_ex();

    return A;
}
void sagtlib::sagent::destroy(sagtlib::sagent* instance){
    delete instance;
}
sagtlib::sagent::~sagent() {}
 