//#include "../data/sydata.h"
//#include <sstream>
//#include <string>
//#include "../mannager/smannager.h"
#include <iostream>
#include "syagent_interface.h"
#include "../agent/sagent.h"
#include "../local_tool/cli_tool.h"
#include "../utils/json/utils_json.h"

using namespace std;

void sagtlib::sagent::toggle_debug(){
    this->debugger=!this->debugger;
    cout<<"Debug:"<<(this->debugger?"on":"off")<<endl;
}

sagtlib::sagent* sagtlib::sagent::create(const char* home,const string& room){
    sagtlib::Agent* A=new sagtlib::Agent(get_home(home),room);
    REGISTER_TOOL(A,terminal,skill_1);
    REGISTER_TOOL(A,get_image,skill_2);
    return A;
}


void sagtlib::sagent::destroy(sagtlib::sagent* instance){
    delete instance;
}

sagtlib::sagent::~sagent() {}

// sagtlib::sagent_mannager* sagtlib::sagent_mannager::create(){
//     sagtlib::manager* A=new sagtlib::manager();
//     return A;

// }
// void sagtlib::sagent_mannager::destroy(sagtlib::sagent_mannager* A){
//     delete A ;
// }
// sagtlib::sagent_mannager::~sagent_mannager(){
    
// }
