#include <sagtapi.h>
#include <iostream>
using namespace std;
//~/.local/share
#ifdef _WIN32
#define DEFAULT_ROOM "windows-agent"
#else 
#define DEFAULT_ROOM "linux-agent"
#endif
int main(int argc,const char** argv) {//usage sample: ./main -home -room -port 
    if(argc==1){
        std::cout\
        <<"Warning: Better Specify Paramters ,otherwise default parameter is Engaged\n"\
        <<"         -p1          --the folder where application workspace will be created at(everything will be stored here)\n"\
        <<"         -p2          --the name of agent, cache file will be store at p1/p2/\n"\
        <<"         -p3          --Initialize app as server, can be done as well if not specify later in the menu\n";
    }
    sagtlib::sagent* agent = sagtlib::sagent::create((argc<2?argv[0]:argv[1]),(argc<3?DEFAULT_ROOM:argv[2]));
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
