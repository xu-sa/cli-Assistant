#ifndef UTILS_2
#define UTILS_2
#include "../../json/json.hpp"
#include <string>
nlohmann::json handle_post(const std::string& provider_url,const std::string& apikey,nlohmann::json* data);//post json to server
#endif