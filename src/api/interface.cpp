
#include <iostream>
#include "syagent_interface.h"
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
    
    {
        DEFINE_TOOL(
            terminal_,
            "To execute a shell command on the local linux/windows machine,use for General tasks, Always use '&&' for Multiple commands Execution in one time",
            {"1","command",STRING_PARAMETER,"The shell command to be Executed", ""}
        )
        REGISTER_TOOL(A,terminal_,skill_1);//tag 0
    }
    {
        std::function<std::string(const std::string*)> func1 = std::bind(&sagtlib::Agent::get_skills, A, std::placeholders::_1);
        DEFINE_TOOL(
            help_,
            "to see what dynamically-registered(DR) tools are available to be activated",
            {"1","show",INT_PARAMETER,"1 to show all DR tool with tags on, or 0 to do nothing", "1 0"}
        )
        REGISTER_TOOL(A,help_,func1);//tag 1
    }
    {
        std::function<std::string(const std::string*)> func2 = std::bind(&sagtlib::Agent::activate_tool, A, std::placeholders::_1);
        DEFINE_TOOL(
            select_,
            "To select dynamically-registered(DR) tool to activate by tag for a short period which you might choose to run",
            {"1","tool_tag",INT_PARAMETER,"the tag of which the DR tool to be activated", ""}
        )
        REGISTER_TOOL(A,select_,func2);//tag 2
    }
    return A;
}
void sagtlib::sagent::destroy(sagtlib::sagent* instance){
    delete instance;
}
sagtlib::sagent::~sagent() {}
 