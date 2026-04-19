#ifndef SYDATA
#define SYDATA
#define PROVIDER_SIZE sizeof(urls)/sizeof(urls[0])
#define MODEL_OPTION 5
#define OPEN_ROUTE_SIZE sizeof(models_open_router) / sizeof(models_open_router[0])
#define LOCAL_USER "super user"
#define COLOR_RESET   "\033[0m"
#define TOOL_FAILED_LIM 3
#define TOOL_CALL_LIM 10
#define TERMINAL_CALL_LIM 6
// 前景色
#define COLOR_BLACK   "\033[30m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_WHITE   "\033[37m"

// 背景色
#define BG_BLACK   "\033[40m"
#define BG_RED     "\033[41m"
#define BG_GREEN   "\033[42m"
#define BG_YELLOW  "\033[43m"
#define BG_BLUE    "\033[44m"
#define BG_MAGENTA "\033[45m"
#define BG_CYAN    "\033[46m"
#define BG_WHITE   "\033[47m"

// 属性
#define BOLD        "\033[1m"
#define UNDERLINE   "\033[4m"
#define REVERSED    "\033[7m"

// 常用组合
#define ERROR_COLOR   COLOR_RED BOLD
#define WARN_COLOR    COLOR_YELLOW BOLD
#define INFO_COLOR    COLOR_CYAN BOLD
#define SUCCESS_COLOR COLOR_GREEN BOLD
#define BLUE_BOLD COLOR_BLUE BOLD

#define MES_0_0 std::cout<<"updated tool "<<definition["function"]["name"]<<"\n";
#define MES_0_1 std::cout<<SUCCESS_COLOR<<"Registered tool "<<COLOR_RESET<<tool_size<<": "<<definition["function"]["name"]<<" "<<(new_skill.state?"+":"-")<<"\n";
#define MES_0_2 string(INFO_COLOR)+"(calling tool)"+this->profile.name+string(COLOR_RESET)+string(" : ")+(message_reply.empty()?"..":message_reply);
#define MES_0_3 string(ERROR_COLOR)+"LLM error"+string(COLOR_RESET)+" : "+to_string(agent_reply["code_"])+" #type:"+to_string(agent_reply["error_type"])+" #message:"+to_string(agent_reply["error_message"]);
#define MES_0_4 string(SUCCESS_COLOR)+this->profile.name+string(COLOR_RESET)+" : "+(message_reply.empty()?"..":message_reply);
#define MES_0_6 std::cout<<"agent service terminated..\n";

#define MES_1_0 std::cout<<"Reading file "<<filename<<"\n";
#define MES_1_1 std::cout<<ERROR_COLOR<<"Error: "<<COLOR_RESET<<"Failed to read file "+filename+"\n";
#define MES_1_2 std::cout<<WARN_COLOR<<"Error: tool must have the same name as it's folder in the definition"<<COLOR_RESET<<"\n";
#define MES_1_3 std::cout<<WARN_COLOR<<"Error: skill type is rather undefined ,no-marching or incorrect"<<COLOR_RESET<<"\n";
#define MES_1_4 std::cout<<WARN_COLOR<<"Error Occurred when registering tool '"<<folderName<<"' : "<<e.what()<<COLOR_RESET<<"\n";
#define MES_1_5 string(WARN_COLOR)+"Error accessing extension Folder(you may need to Configure the extension path): "+string(COLOR_RESET)+ string(e.what())+"\n";
#define MES_1_6 string(SUCCESS_COLOR)+"Loaded agent "+""+this->profile.name+string(COLOR_RESET)+"\n";
#define MES_1_7 "Reloaded chat history : "+I+"\n";


#define MES_1_10 "no such option\n";

#define MES_2_0 "Error: Unrecognizable user id\n"
#define MES_2_1 "Error: Server is busy\n"
#define MES_2_2 std::cout<<"Cant handle such a huge data stream in once"
#define MES_2_3 std::cout<<"main thread is on\n";
#define MES_2_4 "please close current server socket before listening on another socket\n";
#define MES_2_5 "not a correct port number\n";
#define MES_2_6 "server thread start on port "+to_string(this->port_num)+"\n";
#define MES_2_7 "main thread is not been Activated yet..cant start server now.\n";
#define MES_2_8 "server thread shut down..\n";
#define MES_2_9 "server is not been Activated yet..cant join a unjoinable thread\n";
#define MES_2_10 std::cout<<"agent "<<this->profile.name<<"'s main thread is terminated\n";
#define MES_4_0 std::cout << "got one client connecting to socket " << socket_in << "\n";

#include <string>

const std::string models[][5]={
    {"deepseek-chat","deepseek-reasoner","","",""},
    {"gpt-4o","gpt-4o-mini","gpt-4-turbo","",""},
    {"grok-4-1-fast-reasoning","grok-4-1-fast-non-reasoning","grok-imagine-image","",""},
    {"qwen-max","qwen-plus","qwen-turbo","",""},
    {"command-r-plus","command-r","command","command-a-03-2025",""},
    
};

const std::string models_open_router[] = {
    "deepseek/deepseek-chat", "deepseek/deepseek-r1",
    "x-ai/grok-2-1212", "x-ai/grok-2-vision-1212", "x-ai/grok-beta",
    "openai/gpt-4o", "openai/gpt-4o-mini", "openai/o3-mini", "openai/o1",
    "anthropic/claude-3.5-sonnet", "anthropic/claude-3-opus", "anthropic/claude-3.5-haiku",
    "google/gemini-2.5-pro", "google/gemini-2.5-flash",
    "meta-llama/llama-3.3-70b-instruct", "meta-llama/llama-3.1-8b-instruct"
};

const std::string urls[][2]={
    {"https://api.deepseek.com/v1/chat/completions","DeepSeek"},
    {"https://api.openai.com/v1/chat/completions","OpenAI"},
    {"https://api.x.ai/v1/chat/completions","XAI"},
    {"https://dashscope.aliyuncs.com/compatible-mode/v1/chat/completions","Qwen"},
    {"https://api.cohere.ai/compatibility/v1/chat/completions","Cohere"},
    {"https://openrouter.ai/api/v1/chat/completions","OpenRouter"}
};

#endif