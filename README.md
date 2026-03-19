# cli-Assistant
a ai tool Designed to help Improve Efficiency working on command line Interface,Main Feature:

  1:gives you the Ability of Building your own tool in CPP (LLM skill/function calling) 
  2:Execute any commands by itself
  3:send message with Image

How to use:
first of all , this Repository is just a CPP Lib which gives a super user friendly way to embed AI agent into your workflow.
you need to build this project with Cmake on your own OR download the Release Compressed file to get the Archive file and the header file which tells you how to use.
once you have both files here is a easy sample of how to use:
#Sample
```
#include "./lib/syagent_interface.h"
#include <iostream>
#include <string>
using namespace std;
std::string generateRandomMatrix(const string data[]){.....} 
int main(int argc,const char** argv) {
    DEFINE_TOOL(//here you can define your own tool which can be used in LLM tool calling
        get_ramdon_matrix,//the name of tool
        "a user Specified function, to Generate a ramdon matrix", //Description of tool
        {"1","min_row",INT_PARAMETER,"the Minimum of row",""}, //Parameters
        {"1","max_row",INT_PARAMETER,"the Maximum of row",""},//Parameter
        {"1","min_col",INT_PARAMETER,"the Minimum of Column",""},//Parameter
        {"1","max_col",INT_PARAMETER,"the Maximum of Column",""},//Parameter
        {"1","data_type",STRING_PARAMETER,"the type of data","int float"}//Parameter
    )
    sagtlib::sagent* agent = sagtlib::sagent::create(argv[0],"Agent_A");
    if (agent!=NULL) {
        agent->toggle_debug();
        std::cout << "Instance created successfully!" << std::endl;
        REGISTER_TOOL(agent,get_ramdon_matrix,generateRandomMatrix);
        agent->direct_chat_session();//this is a Default way to open a AI chat session,
        sagtlib::sagent::destroy(agent);
        std::cout << "Instance terminated." << std::endl;
    } else {
        std::cerr << "Instance to create agent!" << std::endl;
        return 1;
    }
    
    return 0;
}
```
