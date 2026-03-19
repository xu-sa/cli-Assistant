#ifndef SYDATA
#define SYDATA
#define WORKING 0
#define READY 1
#define FAILED 0

#ifdef DEFINE_TOOL
#undef DEFINE_TOOL 
#define DEFINE_TOOL(Toolname,description,...) const std::string Toolname[][5]={{#Toolname,description,"","",""},__VA_ARGS__};
#endif

#ifdef REGISTER_TOOL
#undef REGISTER_TOOL
#define REGISTER_TOOL(Agent,Definition,Tool) Agent->register_tool(Definition,sizeof(Definition)/sizeof(Definition[0]),Tool)
#define PROVIDER_SIZE sizeof(urls)/sizeof(urls[0])
#define MODEL_OPTION 5
#define OPEN_ROUTE_SIZE sizeof(models_open_router) / sizeof(models_open_router[0])
#endif

#include "../json/json.hpp"
#include <string>
#include <deque>
#include <unordered_map>
static const std::string base64_chars = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";
static nlohmann::json post_error = {{"error", true},{"message", "Request failed"},{"code", -999},{"data", ""}};

static nlohmann::json default_reply={{"role","assistant"},{"content","ummm..."}};

enum input_from{
    CLT_input,
    NET_input
};
 
struct inputs_{
    std::string message;
    std::string image;
    input_from type;
};  

struct SKILL{
    nlohmann::json definition;
    std::function<std::string(const std::string*)> Actual_tool;
    bool state;
    std::vector<std::string> parameter_format;
};

struct Model_setup{
    std::string name;
    std::string whoyouare;//
    std::string api;
    int provider;//LLM provider,currently Deepseek and xai
    int model;//LLM model,Varies from provider
    int openroute_model;
    int max_tokens;//token limites the costs
    bool stream;//stream output/NOT AVAILABLE YET
    float temperature;//temperature to make LLM speak within a Concentrated or Deviated mind 
    float top_p;//Percentage limitation for the Sum of top Possible output characters
    size_t max_message;//how many meesages will be stored
    std::string tool_choice;//Manage single tool Validity
    int chat_state;
    int working_count;
    int fail_count;
};

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

enum FileCategory {
    NOT_SUPPORTED,
    NO_MARCH,
    IMAGE,
    DOCUMENT,
    AUDIO,
    VIDEO,
    ZIP,
    BIN,
    CATEGORY_COUNT
};

static std::unordered_map<std::string, FileCategory> file_type_map = {
    // image file 12 types
    {".jpg", IMAGE}, {".jpeg", IMAGE}, {".jpe", IMAGE},
    {".jfif", IMAGE}, {".jif", IMAGE}, {".png", IMAGE},
    {".webp", IMAGE}, {".gif", IMAGE}, {".bmp", IMAGE},
    {".dib", IMAGE}, {".tiff", IMAGE}, {".tif", IMAGE},
    
    // text file 28 types
    {".pdf", DOCUMENT}, {".txt", DOCUMENT}, {".text", DOCUMENT},
    {".csv", DOCUMENT}, {".pptx", DOCUMENT}, {".ppt", DOCUMENT},
    {".docx", DOCUMENT}, {".doc", DOCUMENT}, {".xlsx", DOCUMENT},
    {".xls", DOCUMENT}, {".html", DOCUMENT}, {".htm", DOCUMENT},
    {".json", DOCUMENT}, {".h", DOCUMENT}, {".cpp", DOCUMENT},
    {".py", DOCUMENT}, {".java", DOCUMENT}, {".js", DOCUMENT},
    {".ts", DOCUMENT}, {".c", DOCUMENT}, {".cs", DOCUMENT},
    {".php", DOCUMENT}, {".rb", DOCUMENT}, {".go", DOCUMENT},
    {".rs", DOCUMENT}, {".swift", DOCUMENT}, {".kt", DOCUMENT},
    {".md", DOCUMENT},
    
    // audio file 9 types
    {".mp3", AUDIO}, {".mp4", AUDIO}, {".m4a", AUDIO},
    {".wav", AUDIO}, {".flac", AUDIO}, {".aac", AUDIO},
    {".ogg", AUDIO}, {".oga", AUDIO}, {".wma", AUDIO},
    
    // video file 8 types
    {".mp4", VIDEO}, {".m4v", VIDEO}, {".mov", VIDEO},
    {".avi", VIDEO}, {".mkv", VIDEO}, {".webm", VIDEO},
    {".flv", VIDEO}, {".wmv", VIDEO},
    
    // zip files 
    {".zip", ZIP}, {".rar", ZIP}, {".7z", ZIP},
    {".tar", ZIP}, {".gz", ZIP},
    
    // binary files
    {".exe", BIN}, {".dll", BIN}, {".bin", BIN}
};
 
#endif