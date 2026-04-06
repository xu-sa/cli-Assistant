#ifndef SYDATA
#define SYDATA
#define PROVIDER_SIZE sizeof(urls)/sizeof(urls[0])
#define MODEL_OPTION 5
#define OPEN_ROUTE_SIZE sizeof(models_open_router) / sizeof(models_open_router[0])

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