#ifndef UTILS_2
#define UTILS_2
#ifdef _WIN32
#include "../../lib-w/json/json.hpp"
#else
#include "../../lib-l/json/json.hpp"
#endif
#include <string>
nlohmann::json handle_post(const std::string& provider_url,const std::string& apikey,nlohmann::json* data);//post json to server
#endif