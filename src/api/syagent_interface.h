#ifndef SYAGENT_INTERFACE
#define SYAGENT_INTERFACE

#define INT_PARAMETER "integer"
#define STRING_PARAMETER "string"
#define NUMBER_PARAMETER "number"

#define BOOL_PARAMETER "boolean"
#define ARRAY_PARAMETER "array"
#define OBJECT_PARAMETER "object"
#define NULL_PARAMETER "null"

#define DEFINE_TOOL(Toolname,description,...) const std::string Toolname[][5]={{#Toolname,description,"","",""},__VA_ARGS__};// to define a tool from a Customized function with any discription and parameter definition.
#define REGISTER_TOOL(Agent,Definition,Tool) Agent->register_tool(Definition,sizeof(Definition)/sizeof(Definition[0]),Tool);// register the pre-defined tools in DEFINE_TOOL.

#include <string>
#include <functional>
namespace sagtlib{
    class sagent {
    public:
        bool debugger;
        virtual ~sagent();
        virtual void register_tool(const std::string definition_[][5],size_t,std::function<std::string(const std::string*)> func) = 0;
        virtual void start_server_thread(std::string& port)=0;
        virtual void stop_server_thread()=0;
        virtual void push_input(int cli_or_web,const std::string& text)=0;//CLI_or_web defines to where the output and Reply be printed
        virtual void interface()=0;//to start a termianl-only session for chat Process 
        int port_num;//the program listen on this port
        void toggle_debug();//this will give more outputs for referring working states in terminal
        static sagent* create(const char* home,const std::string& room); //-home: indicates the workspace path, recommend to use it as the argument from main entrance function. -room: the name of the agent's setting,Memory files and chat history , will create a new one if not exsist within the '-home' 
        static void destroy(sagent*);
    };
    // class sagent_mannager{
    // public:
    //     virtual ~sagent_mannager();
    //     virtual void add_agent()=0;
    //     virtual void remove_agent()=0;
    //     virtual void new_session()=0;
    //     static sagent_mannager* create();
    //     static void destroy(sagent_mannager*);
    // };
}

#endif
