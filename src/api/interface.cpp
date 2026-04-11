
#include <iostream>
#include "syagent_interface.h"
#include "../agent/sagent.h"
#include "../local_tool/cli_tool.h"
#include "../utils/main/utils_1.h"
#define PRINT_ERROR std::cout<<"Unexpected Error Occurred : 9";
using namespace std;

string extension_home="";

// void sagtlib::set_extension_env(const char* a){
//     extension_home=build_path(get_home(a),".extension");   
// }
void sagtlib::sagent::toggle_debug(){
    this->debugger=!this->debugger;
    cout<<"Debug:"<<(this->debugger?"on":"off")<<endl;
}
sagtlib::sagent* sagtlib::sagent::create(const char* home,const string& room){
    Agent* A=new Agent(build_path(get_home(home),".slcache"),room);
    DEFINE_TOOL(
        terminal,
        "To execute a shell command on the local linux/windows machine,use for General tasks, Always use '&&' for Multiple commands Execution in one time",
        {"1","command","string","The shell command to be Executed", ""}
    )
    REGISTER_TOOL(A,terminal,skill_1);
    return A;
}
void sagtlib::sagent::destroy(sagtlib::sagent* instance){
    delete instance;
}
sagtlib::sagent::~sagent() {}
 