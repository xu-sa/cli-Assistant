#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cstdlib>
#include <unordered_map>
#include <curl/curl.h>
#include "string.h"
#include "utils.h"
#include "../json/json.hpp"
#include "../data/sydata.h"
#define sleep_1 usleep(100 * 1000);

using json=nlohmann::json;
using namespace std;
namespace fs = std::filesystem;

//C Style

// std::string base64_encode(char* data,size_t size_) {
//     std::string ret;
//     int i = 0;
//     int j = 0;
//     unsigned char char_array_3[3];
//     unsigned char char_array_4[4];
//     for (size_t n = 0; n < size_; ++n) {
//         char_array_3[i] = data[n];
//         i++;
//         if (i == 3) {
//             encode_triplet(char_array_3,char_array_4);
//             for (i = 0; i < 4; i++) ret += base64_chars[char_array_4[i]];
//             i = 0;
//         }
//     }
//     for(size_t k=i;k<3;k++)char_array_3[k]=0b0;//C++ 14
//     encode_triplet(char_array_3,char_array_4);
//     for(size_t k=0;k<4;k++)ret +=(k<i+1?base64_chars[char_array_4[k]]:'=');//remain n -> use n+1 to contain, index 0 to n
//     return "base64,"+ret;
// }

// std::string detect_image_format(char* data) {
//     if (data[0] == (char)0xFF && data[1] == (char)0xD8) return "data:image/jpeg;";
//     if (data[0] == (char)0x89 && data[1] == (char)0x50 && 
//         data[2] == (char)0x4E && data[3] == (char)0x47) return "data:image/png;";
//     if (data[0] == 'G' && data[1] == 'I' && data[2] == 'F') return "data:image/gif;";
//     if (data[0] == 'R' && data[1] == 'I' && 
//         data[2] == 'F' && data[3] == 'F') return "data:image/webp;";
//     return "";  
// }

static void encode_triplet(unsigned char char_array_3[3], unsigned char char_array_4[4]) {
    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;
}
 
static std::string base64_encode(const std::vector<unsigned char>& data) {
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3]={0};
    unsigned char char_array_4[4]={0};
    for (size_t n = 0; n < data.size(); ++n) {
        char_array_3[i] = data[n];
        i++;
        if (i == 3) {
            encode_triplet(char_array_3,char_array_4);
            for (i = 0; i < 4; i++) ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }
    for(size_t k=i;k<3;k++)char_array_3[k]=0b0;//C++ 14
    encode_triplet(char_array_3,char_array_4);
    for(size_t k=0;k<4;k++)ret +=(k<i+1?base64_chars[char_array_4[k]]:'=');//remain n -> use n+1 to contain, index 0 to n
    return "base64,"+ret;
}

static std::string detect_image_format(std::vector<unsigned char>& data) {
    if ((char)data[0] == (char)0xFF && (char)data[1] == (char)0xD8) return "data:image/jpeg;";
    if ((char)data[0] == (char)0x89 && (char)data[1] == (char)0x50 && 
        (char)data[2] == (char)0x4E && (char)data[3] == (char)0x47) return "data:image/png;";
    if ((char)data[0] == 'G' && (char)data[1] == 'I' && (char)data[2] == 'F') return "data:image/gif;";
    if ((char)data[0] == 'R' && (char)data[1] == 'I' && 
        (char)data[2] == 'F' && (char)data[3] == 'F') return "data:image/webp;";
    return "";
}

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    std::string* str = (std::string*)userp; 
    str->append((char*)contents, totalSize);
    return totalSize;
}

//main tool
json handle_read_json(const string& home,const string& room,const string& filename){
    json data={};
    fs::path file_in_working_folder = fs::path(home)/room/"file_test_if_Exists.txt"; 
    fs::create_directories(file_in_working_folder.parent_path());
    if(filename.empty())return data;
    fs::path p=fs::path(home)/room  / filename;
    
    std::ifstream file(p.string());
 
    try{
        data = json::parse(file);
        cout<<"System : Reading file "<<room<<"/"<<filename<<endl;
    }
    catch(const json::parse_error& e){
        cout<<"System : Failed to read file\n";
    }
    file.close();
    return data;
}

json handle_registration(const string definition_[][5],size_t amount,vector<string >* parameter_format){
    json definition;
    try{
        vector<string> required;
        json properties;
        for(size_t sz=1;sz<amount;sz++){
            if(definition_[sz][0]=="1")required.push_back(definition_[sz][1]);
            properties[definition_[sz][1]]={{"type",definition_[sz][2]},{"description",definition_[sz][3]}};
            if(definition_[sz][4]!=""){
                vector<string> enum_;
                stringstream ss(definition_[sz][4]);
                string choice;
                while (ss >> choice) {
                    enum_.push_back(choice);
                }
            properties[definition_[sz][1]]["enum"]=enum_;
            };
            parameter_format->push_back(definition_[sz][1]);    
        };
        definition={
            {"type","function"},
            {"function",{
                {"name",definition_[0][0]},
                {"description",definition_[0][1]},
                {"parameters",{
                    {"type","object"},
                    {"properties",properties},
                    {"required", required}
                    }
                }
                }
            }};
    }
    catch (const std::exception& e) {
        definition["type"]="Wrong";
        std::cerr << "Exception Occurred When Parsing a tool definition(Escaped): " << e.what() << std::endl;
    };
    return definition;
};

void handle_save(json* save,const string& home,const string& room,const string& filename){
    fs::path save_path;
    fs::path file_in_working_folder = fs::path(home)/room / "file_test_if_Exists.txt"; 
    fs::create_directories(file_in_working_folder.parent_path());
    save_path=fs::path(home) /room/ filename;
    sleep_1
    std::ofstream file(save_path.string());
    if (file.is_open()) {
        file << save->dump(4);
        file.close();
        cout<<"System: Succeed Caching "<<room<<"/"<<filename<<endl;
        return;
    }
    std::cerr << "System: Failed to save "<<room<<"/"<< filename << std::endl;
    return;
}

json handle_post(const string& provider,const string& apikey,json* jsonbody){
    CURL* curl=curl_easy_init();
    struct curl_slist *headers = NULL;
    json result;
    try{
        string response;
        std::string json_body=jsonbody->dump();
        if(!curl)return post_error;
        curl_easy_setopt(curl, CURLOPT_URL, provider.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        headers = curl_slist_append(headers, ("Authorization: Bearer " + apikey).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");
        if(!headers)return post_error;
        curl_easy_setopt(curl,CURLOPT_HTTPHEADER,headers);
        curl_easy_setopt(curl,CURLOPT_POSTFIELDS,json_body.c_str());
        curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,WriteCallback);
        curl_easy_setopt(curl,CURLOPT_WRITEDATA, &response);
        CURLcode res=curl_easy_perform(curl);
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        if(http_code==200){
            result=json::parse(response)["choices"][0]["message"];
            result["code_"]=200;
            result["error_message"]="None";
            result["error_type"]="None";
         //   cout<<'\n'<<response<<'\n';
        }
        else {
            try {
                json error_response = json::parse(response);
                if(error_response.contains("error")) {
                    result["error_message"]=(error_response["error"].contains("message")?error_response["error"]["message"].get<string>() : "Unknown error");
                    result["error_type"]=(error_response["error"].contains("type")?error_response["error"]["type"].get<string>() : "");
                } 
                else{
                    result["error_message"]="Unknown error";
                    result["error_type"]="Unknown error";
                }
            } 
            catch(const json::exception& e) 
            {
                result["error_message"]="Unknown error";
                result["error_type"]="Unknown type";
            }
            result["code_"] = http_code;
        }
        
    }catch(const std::exception& e){
        result=post_error;
        result["code_"]=-999;
        result["error_message"]="\"Least Favored Issue\"";
        result["error_type"]="Unknown type";
        cout<<"Critical Error Occur when parsing Data:\n"<<e.what()<<endl;
    }
    
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return result;
}

//file handler

int check_file_type(const string& path){
    size_t dotPos = path.find_last_of('.');
    if (dotPos == std::string::npos || dotPos == path.size()-1)return (int)NOT_SUPPORTED;
    string I=path.substr(dotPos);
    auto it = file_type_map.find(I);
    if(it!=file_type_map.end()){
        return (int)it->second;
    }else return (int)NO_MARCH;
}

std::string decode_image(const std::string& filepath) {
    string i="";
    if(filepath=="")return i;
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Cant open this picture: " << filepath << std::endl;
        return i;
    }
    file.seekg(0,ios::end);//set position to the end 
    std::streamsize size = file.tellg();//return the stream position in the Form of offset from begin (size)
    std::vector<unsigned char> buffer(size);
    file.seekg(0,ios::beg);
    if (file.read((char*)buffer.data(), size)){
        i+=detect_image_format(buffer);
        if(i!="")i+=base64_encode(buffer);
    }
    file.close();
    return i;
}

std::string decode_txt(const std::string& path) {    
    std::ifstream file(path);
    if (!file.is_open()) {
        return "**cant open file: " + path + "**";
    }
    
    std::string content;// data stored on the heap memory
    std::string line;
    while (std::getline(file, line)) {
        content += line + "\n";
    }
    
    file.close();

    const size_t MAX_LENGTH = 1024*10;
    if (content.length() > MAX_LENGTH) {
        content = content.substr(0, MAX_LENGTH) + "\n...(file is too long to view here)";
    }
    
    return "**user send a text file**:\n```\n" + content + "\n```";
}

std::string decode_audio(const string& path){
    return "**user send a audio file but currently cant parse this audio file**";
}

std::string decode_video(const string& path){
    return "**user send a video file but currently cant parse this video file**";
}

//Miscellaneous tools
string get_home(const char* argument_0){
    fs::path p =fs::absolute(fs::path(argument_0)).parent_path();
    return (p / ".Slcache").string();
}

string get_time(){
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss<< std::put_time(std::localtime(&time_t_now),("%m_%d_%H_%M_"));
    return ss.str();
}

string get_env(int provider){
    char buffer[256];
    switch (provider)
    {
    case 0:
        snprintf(buffer,sizeof(buffer),"%s",std::getenv("DEEPSEEK_API_KEY"));
        break;
    case 1:
        snprintf(buffer,sizeof(buffer),"%s",std::getenv("OPENAI_API_KEY"));
        break;
    case 2:
        snprintf(buffer,sizeof(buffer),"%s",std::getenv("XAI_API_KEY"));
        break;
    case 3:
        snprintf(buffer,sizeof(buffer),"%s",std::getenv("QWEN_API_KEY"));
        break;
    case 4:
        snprintf(buffer,sizeof(buffer),"%s",std::getenv("COHERE_API_KEY"));
        break;
    case 5:
        snprintf(buffer,sizeof(buffer),"%s",std::getenv("OPENROUTE_API_KEY"));
        break;
    default:
        break;
    }
    return buffer;
}

int set_int(const std::string& str){
    try{
        int i =0;
        i=stoi(str);
        return i;
    }catch(const exception e){
        return -23;
    }
}

float set_float(const string& value){

    float value_float;
    stringstream ss(value);
    ss>>value_float;
    if (ss.fail() || !ss.eof()) {
        value_float=-24.0f;
    } 
    return value_float;
}
