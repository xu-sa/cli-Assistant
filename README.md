# cli-Assistant
a ai tool Designed to help Improve Efficiency working on command line Interface

# Main Feature:
  
1:gives you the Ability of Building your own tool in CPP (LLM skill/function calling)

2:Execute any commands by itself

3:send Multiple types of file via termial 

# Prerequisite: nlohmann::json & curl

# How to use:

first of all , this Repository is just a CPP Lib which gives a super user friendly way to embed AI agent into your workflow.

you need to build this project with Cmake on your own OR download the Release Compressed file to get the Archive file and the header file which tells you how to use.

once you have both files here is a easy sample of how to use:

```
#include "syagent_interface.h"
#include <iostream>
using namespace std;
int main(int argc,const char** argv) {
    sagtlib::sagent* agent = sagtlib::sagent::create(argv[0],"Agent_A");
    if (agent!=NULL) {
        agent->interface();// this will start the Termial Interactive Console
        sagtlib::sagent::destroy(agent);
    } else {
        std::cerr << "failed to create agent!" << std::endl;
        return 1;
    }    
    return 0;
}
```
# Terminal Interface:
you can use and find Instruction about commands and configs once you have the sagent::interface() function running
