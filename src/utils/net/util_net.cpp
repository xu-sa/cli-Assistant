#include "utils_net.h"
#include <string>
#include <iostream>
#include <curl/curl.h>
#include "../../data/sydata.h"
#define PRINT_ERROR std::cout<<"Unexpected Error Occurred : 13";

using namespace std;
using json=nlohmann::json;
static nlohmann::json post_error = {{"error", true},{"message", "Request failed"},{"code", -999},{"data", ""}};

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    std::string* str = (std::string*)userp; 
    str->append((char*)contents, totalSize);
    return totalSize;
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
            //cout<<'\n'<<response<<'\n';
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

