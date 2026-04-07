#include <iostream>
#include <fstream>
#ifdef _WIN32
    #include <windows.h>
    #define sleep_1 Sleep(300);
#else
    #include <unistd.h>
    #define sleep_1 usleep(300 * 1000);
#endif
#include "utils_json.h"

using json=nlohmann::json;
using namespace std;
namespace fs = std::filesystem;
#define PRINT_ERROR std::cout<<"Unexpected Error Occurred : 12";

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
        cout<<"Failed to read file "+filename+"\n";
        data={};
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

string handle_get_history(const string& home, const string& room,int choice) {
    fs::path file_in_working_folder = fs::path(home) / room / "file_test_if_Exists.txt";
    fs::create_directories(file_in_working_folder.parent_path());
    fs::path dir_path = fs::path(home) / room;
    int count=0;
    string message="please specify a tag of history file:\n";
    //vector<string> json_files;
    if (fs::exists(dir_path) && fs::is_directory(dir_path)) {
        for (const auto& entry : fs::directory_iterator(dir_path)) {
            if (entry.is_regular_file()) {
                string filename = entry.path().filename().string();
                if (filename.find("_chat.json") == string::npos)continue;//if name is not in correct format
                if(fs::file_size(entry.path())>1024*1024*100)continue;//if >10Mb then pass
                if(count==choice)return filename;
                message+="       "+to_string(count)+". "+filename+"\n";
                count++;
            }
        }
    }//if none tag satisfied 
    message+=(count==0?"no cached history found\n":"choose from these tags\n");
    return '\n'+message;
}

string handle_save(json* save,const string& home,const string& room,const string& filename){
    fs::path save_path;
    fs::path file_in_working_folder = fs::path(home)/room / "file_test_if_Exists.txt"; 
    fs::create_directories(file_in_working_folder.parent_path());
    save_path=fs::path(home) /room/ filename;
    sleep_1
    std::ofstream file(save_path.string());
    if (file.is_open()) {
        file << save->dump(4);
        file.close();
        return "Succeed Caching "+home+"/"+room+"/"+filename+"\n";
    }
    return "Failed saving to "+home+"/"+room+"/"+filename+"\n";
}

//Miscellaneous tools
string get_home(const char* argument_0){
    fs::path p =fs::absolute(fs::path(argument_0)).parent_path();
    return (p / ".slcache").string();
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
