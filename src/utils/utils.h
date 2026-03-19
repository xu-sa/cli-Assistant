#ifndef UTILS
#define UTILS
#include <iostream>
#include <string>
#include "../json/json.hpp"

std::string decode_image(const std::string& filepath);//decode image as base64 Sequence
std::string decode_audio(const std::string& filepath);//decode mp3 file into string
std::string decode_txt(const std::string& filepath);//copy the text into string
std::string decode_video(const std::string& filepath);//decode file into...
int check_file_type(const std::string& path);//return FileCategory 

nlohmann::json handle_post(const std::string& provider_url,const std::string& apikey,nlohmann::json* data);//post json to server
nlohmann::json handle_read_json(const std::string& home,const std::string& room,const std::string& name);//read local config and chat history
nlohmann::json handle_registration(const std::string definition_[][5],size_t,std::vector<std::string>*);//register tool
void handle_save(nlohmann::json* save, const std::string& home,const std::string& room,const std::string& filename);//save chat or config to json file

//miscellaneous tools
std::string get_env(int provider);
std::string get_time();
std::string get_home(const char* argument_0);//build a home path
int set_int(const std::string& str);
float set_float(const std::string& str);

#endif