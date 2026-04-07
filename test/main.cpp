#include "./lib/syagent_interface.h"
#include <iostream>
using namespace std;
//~/.local/share
#ifdef _WIN32
#define DEFAULT_HOME argv[0]
#define DEFAULT_ROOM "windows-agent"
#define DEFAULT_PORT 9995
#else 
#define DEFAULT_HOME argv[0]
#define DEFAULT_ROOM "linux-agent"
#define DEFAULT_PORT 9995
#endif
int main(int argc,const char** argv) {//usage sample: ./main -home -room -port 
    sagtlib::sagent* agent = sagtlib::sagent::create((argc<2?DEFAULT_HOME:argv[1]),(argc<3?DEFAULT_ROOM:argv[2]));
    if (agent!=NULL) {
        if(argc<4){
            agent->terminalsession(1);
        }else{
            agent->start_server_thread(argv[3]);    
            agent->terminalsession(0);
        }
        sagtlib::sagent::destroy(agent);
    } else {
        std::cerr << "failed to create agent!" << std::endl;
        return 1;
    }    
    return 0;
} 