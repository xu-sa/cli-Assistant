//Main function Logic
#include "sagent.h"
#include <iostream>
#include <fstream>
#include "../utils/main/utils_1.h"
#include "../local_tool/cli_tool.h"
#define PRINT_ERROR std::cout<<"Unexpected Error Occurred : 5";
using namespace std;
using json=nlohmann::json;
namespace fs = std::filesystem;

static string check_extension_system(const std::string& path) {
    std::string res="";
    try {
        for (const auto& entry : fs::directory_iterator(path)) {
            if (entry.is_directory()) {
                std::string folderName = entry.path().filename().string();
                bool hasPy = false;
                bool hasMd = false;
                for (const auto& fileEntry : fs::directory_iterator(entry.path())) {
                    if (fileEntry.is_regular_file()) {
                        std::string filename = fileEntry.path().filename().string();
                        std::string stem = fileEntry.path().stem().string();
                        
                        if (stem == folderName) {
                            if (fileEntry.path().extension() == ".py") hasPy = true;
                            if (fileEntry.path().extension() == ".json") hasMd = true;
                        }
                    }
                }
                
                res+=(hasPy&&hasMd?folderName+" ":"");
            }
        }
        //std::cout<<"Loading skills: "<<res<<"\n";
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error accessing path: " << e.what() << std::endl;
    }
    size_t sz=res.length();
    if(sz>=200){
        std::cerr<<"Error : too many skills,Aborting skill Registration..Please check your extension folder\n";
        res="help ";
    }
    return res;
}

static json handle_read_json(const string& filename){
    json data={};
    fs::path p = filename;
    fs::create_directories(p.parent_path());
    if(filename.empty())return data;
    std::ifstream file(p.string());
    try{
        data = json::parse(file);
        cout<<"System : Reading file "<<filename<<"\n";
    }
    catch(const json::parse_error& e){
        cout<<"Failed to read file "+filename+"\n";
        data={};
    }
    file.close();
    return data;
}

static string handle_get_history(const string& home, const string& room,int choice) {
    fs::path dir_path = fs::path(home) / room;
    int count=0;
    string message="please specify a tag of history file:\n";
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

static string handle_save(json* save,const string& home,const string& room,const string& filename){
    fs::path save_path;
    fs::path file_in_working_folder = fs::path(home)/room / "file_test_if_Exists.txt"; 
    fs::create_directories(file_in_working_folder.parent_path());
    save_path=fs::path(home) /room/ filename;
    sleep_2(2)
    std::ofstream file(save_path.string());
    if (file.is_open()) {
        file << save->dump(4);
        file.close();
        return "Succeed Caching "+home+"/"+room+"/"+filename+"\n";
    }
    return "Failed saving to "+home+"/"+room+"/"+filename+"\n";
}

string sagtlib::Agent::load_ex(const string& extension_h){
    if(!extension_h.empty()){
        extension_home=extension_h;
        std::cout<<"extension home changed to "<<extension_h<<"\n";
    };
    string f=check_extension_system(extension_home);
    this->pyskill_env=extension_home;
    DEFINE_TOOL(
        pyskill,
        "Unified Standard entry for Multiple python skills",
        {"1", "script_name", "string", "The python skill name", f},
        {"1", "json_body", "string", "JSON formatted arguments,Varies from skill you selected,use '{}' for hint", ""}
    )
    REGISTER_TOOL(this,pyskill,skill_2);
    return "reloaded python extension: "+f+"\n";
}

string sagtlib::Agent::load_cf(){
    {//the Model setup
        json data=handle_read_json(build_path(build_path(this->home,this->room),"profile.json"));//build the home/room/profile.json path and read file
        this->profile.api = (data.contains("APIKEY")?data["APIKEY"]:"");
        this->pyskill_env=(data.contains("pyskill")?data["pyskill"]:"");
        this->profile.name = this->room;
        this->profile.tool_choice = (data.contains("tool_choice")?data["tool_choice"]:"auto");
        //this->profile.whoyouare = (data.contains("whoyouare")?data["whoyouare"]:"you are a helpful AI Assistant");
        try{
            this->profile.temperature = (data.contains("temperature") ? (float)data["temperature"] : 0.7f);
            this->profile.top_p = (data.contains("top_p") ? (float)data["top_p"] : 0.8f);
            this->profile.provider = (data.contains("PROVIDER_1") ? (int)data["PROVIDER_1"] : 0);
            this->profile.model = (data.contains("MODEL") ? (int)data["MODEL"] : 0);
            this->profile.openroute_model = (data.contains("OPEN_ROUTE_MODEL") ? (int)data["OPEN_ROUTE_MODEL"] : 0);
            this->profile.max_tokens=(data.contains("max_tokens")?(int)data["max_tokens"]:2000);
            this->profile.max_message=(data.contains("max_history")?(int)data["max_history"]:40);
            this->profile.stream = (data.contains("stream") ? (bool)data["stream"] : false );
            this->profile.local_llm_socket=(data.contains("local_socket")?(int)data["local_socket"]:-1);
        }
        catch(...){
            cout<<"Initiation: Config Variable formate issue Occurred,using Default...\n";
            this->profile.temperature = 0.7f;
            this->profile.top_p = 0.8f;
            this->profile.provider = 0;
            this->profile.model = 0;
            this->profile.openroute_model = 0;
            this->profile.max_tokens=2000;
            this->profile.max_message=40;
            this->profile.stream = false;
            this->profile.local_llm_socket=-1;
        }
        if(this->profile.max_message>150)this->profile.max_message=150;
        if(this->profile.max_message<10)this->profile.max_message=10;
    }
    
    {//message vector
        if(!this->message_pool.empty()){//clean up message
        this->message_pool.clear();
        this->message_pool.shrink_to_fit();
        }
        this->message_pool.push_back({
            {"role","system"},
            {"content",(
                "**your name**:"+this->profile.name+\
                "\n **who you are**:"+read_as_string(build_path(build_path(this->home,this->room),"IDENTITY.md"))+\
                "\n **General RULE**:"+read_as_string(build_path(this->home,"RULE.md"))+\
                "\n **your Additional RULE**:"+read_as_string(build_path(build_path(this->home,this->room),"SUBRULE.md"))
            )}
        });
    
    }
    
    {//input buffer 
        this->chat_state = 1;
        this->working_count = 0;
        this->fail_count = 0;
    }
    
    this->on=1;
    return "Initiated agent "+this->profile.name+"\n";
}

string sagtlib::Agent::load_ch(const string& option){
    int choice;
    try{
        choice=stoi(option);
    }catch(const exception e){
        choice=-23;
    }
    string I=handle_get_history(this->home,this->room,choice);
    if(I[0]=='\n')return I;
    json data=handle_read_json(
        build_path(build_path(this->home,this->room),I)
    );
    if(data.empty())return "cant use this file as reload chat\n";
    this->message_pool=data;
    return "Reloaded chat history : "+I+"\n";
}

string sagtlib::Agent::save(const string& choice){
    json save;
    if(choice=="h"){//save chat history
        save=this->message_pool;
        return handle_save(&save,this->home,this->room,(get_time()+"chat.json"));
    }
    else if(choice=="p"){//save config
        //save["whoyouare"]=this->profile.whoyouare;
        save["APIKEY"]=this->profile.api;
        save["PROVIDER_1"]=this->profile.provider;
        save["MODEL"]=this->profile.model;
        save["OPENROUTE_MODEL"]=this->profile.openroute_model;
        save["max_tokens"]=this->profile.max_tokens;
        save["stream"]=this->profile.stream;
        save["temperature"]=this->profile.temperature;
        save["top_p"]=this->profile.top_p;
        save["max_history"]=this->profile.max_message;
        save["tool_choice"]=this->profile.tool_choice;
        save["local_socket"]=this->profile.local_llm_socket;
        save["pyskill"]=this->pyskill_env;
        return handle_save(&save,this->home,this->room,"profile.json");
    }else return "no such option\n";
}

