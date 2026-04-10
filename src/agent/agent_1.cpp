//Main function Logic
#include "sagent.h"
#include <iostream>
#include "../data/sydata.h"
#include "../utils/json/utils_json.h"
#include "../utils/net/utils_net.h"
#define PRINT_ERROR std::cout<<"Unexpected Error Occurred : 5";
using namespace std;
using json=nlohmann::json;
static nlohmann::json default_reply={{"role","assistant"},{"content","ummm..."}};

void sagtlib::Agent::register_tool(const string definition_[][5],size_t amount, std::function<std::string(const std::string*)> Actual_tool) {
    SKILL new_skill;
    json definition;
    definition = handle_registration(definition_,amount,&new_skill.parameter_format);
    if(definition["type"]=="Wrong")return;
    new_skill.definition=definition;
    new_skill.Actual_tool = Actual_tool;
    new_skill.state=true;
    this->SKILLs.push_back(new_skill);
    this->SKILL_map[definition["function"]["name"]]=this->SKILLs.size()-1;
    return;
}

json sagtlib::Agent::run_tool(size_t index,json* data){
    if(this->debugger)cout<<"\nDEBUGGER: Handling Tool Request Data : "<<(*data).dump()<<endl;
    if(this->working_count==6)return{{"role","tool"},{"content","You Have Been Continually Request for Tool for too many times,report to user the situation and ask for next move Instantly"},{"tool_call_id",(*data)["id"]}};
    if(this->fail_count>=4)return{{"role","tool"},{"content","Tool keeps Failing , you may choose to report to user and ask for next step"},{"tool_call_id",(*data)["id"]}};      
    if(!this->SKILLs[index].state) return {{"role","tool"},{"content","The Tool is temporarily Disabled"},{"tool_call_id",(*data)["id"]}};
    json result;
    result["role"]="tool";
    result["tool_call_id"]=(*data)["id"];
    try{
        size_t parameter_amount=this->SKILLs[index].parameter_format.size(); 
        string data_[parameter_amount];
        for(size_t i=0;i<parameter_amount;i++)data_[i]=((*data)["arguments"][this->SKILLs[index].parameter_format[i]].is_null()?"":
            (*data)["arguments"][this->SKILLs[index].parameter_format[i]].get<std::string>());
        
        if(this->debugger){
            cout<<"\nDEBUGGER: using tool : "<<this->SKILLs[index].definition["function"]["name"]<<"\nParameters : ";
            for (const auto& str : data_) {
                std::cout << str << "; ";
            };
        }
        {//get result from tool and build the json
            string res=this->SKILLs[index].Actual_tool(data_);
            if((*data)["name"]=="get_image"){//if using image tool
               //this->image_attached=1;
                json content=json::array();
                content.push_back({{"type","text"},{"text","Image is Fetched Successfully."}});
                content.push_back({{"type", "image_url"},{"image_url",{{"url",res}} }});//data:image/png;base64,iVBO.....
                result["content"]=content;
            }else{
                result["content"]=res;
            }
        }
        this->working_count+=1;
        this->fail_count=0;
    }
    catch(const std::exception& e){
        string res = std::string(e.what());
        result["content"]=res;
        this->working_count=0;
        this->fail_count+=1;
    };
    if(this->debugger)cout<<"\nDEBUGGER: Tool Result : "<<result<<endl;
    return result;
}

json sagtlib::Agent::get_skills(){
    json all=json::array();
    unsigned int skill_number= this->SKILLs.size();
    for(size_t i=0;i<skill_number;i++){
        json def;
        if(this->SKILLs[i].state)def=this->SKILLs[i].definition;
        if(!def.empty() && !def.is_null())all.push_back(def);
    };
    return all;
};

void sagtlib::Agent::handle_tool_request(json* agent_reply){
    this->message_pool.push_back({{"role",(*agent_reply)["role"]},{"content",(*agent_reply)["content"]},{"tool_calls",(*agent_reply)["tool_calls"]}});
    json agent_tool_usage=(*agent_reply)["tool_calls"];
    int tool_call_count = agent_tool_usage.size();
    for(int i=0;i<tool_call_count;i++){
        json data;
        {//to build data
            data["id"]=agent_tool_usage[i]["id"];
            data["name"]=agent_tool_usage[i]["function"]["name"];
            string args_str = agent_tool_usage[i]["function"]["arguments"];
            try {
                data["arguments"] = json::parse(args_str);
            } catch (const json::parse_error& e) {
                data["arguments"] = args_str;
            };
        }
        std::unordered_map<std::string, size_t>::iterator skill_= this->SKILL_map.find(data["name"]);
        this->message_pool.push_back(this->run_tool(skill_->second,&data));sleep_2(3)
    }
};

string sagtlib::Agent::send(){
    string message_reply;
    json request_body;

    if(this->message_pool.size()>=this->profile.max_message){//clean up the Earliest two message when there is too much chat ,Regardless of System Prompt
        this->message_pool.erase(this->message_pool.begin() + 2);
        this->message_pool.erase(this->message_pool.begin() + 1);
        while(this->message_pool.size()>=2&&this->message_pool[1]["role"]=="tool")this->message_pool.erase(this->message_pool.begin()+1);
    };
    
    {//to build the request body
        request_body["model"]=(this->profile.local_llm_socket!=-1?"local llm":(urls[this->profile.provider][1]=="OpenRoute"?models_open_router[this->profile.openroute_model]:models[this->profile.provider][this->profile.model]));
        request_body["messages"]=this->message_pool;
        request_body["temperature"]=this->profile.temperature;  
        request_body["top_p"]=this->profile.top_p;
        request_body["max_tokens"]=this->profile.max_tokens;
        request_body["stream"]=this->profile.stream;
        request_body["tools"] = get_skills();
        request_body["tool_choice"] =this->profile.tool_choice;
    }
    
    if(this->input_pool[this->push_out].image!=""){//if there is a image base64 string in the message
        if(this->message_pool.back()["role"]=="tool"){//and its from a tool
            if(this->debugger)cout<<"cleaning up the image cache from tool calling\n";
            this->message_pool.back()["content"].clear();
            this->message_pool.back()["content"]="\n**there was a one-time-view image , you can require it if you need**";
        }else{//and its from user input
            if(this->debugger)cout<<"cleaning up the image cache from user input\n";
            string message;
            for (const auto& item : this->message_pool.back()["content"]) {
                if (item["type"] == "text")message += item["text"].get<std::string>();
            }
            this->message_pool.back()["content"].clear();
            this->message_pool.back()["content"]=message+"\n**there was a one-time-view image, you can require it if you need**";
        }
    }
    
    json agent_reply = handle_post((this->profile.local_llm_socket==-1?urls[this->profile.provider][0]:"127.0.0.1:"+to_string(this->profile.local_llm_socket)+"/chat/completions"),this->profile.api,&request_body);
    
    if (agent_reply["code_"] == 200) {
        string reply_context=agent_reply["content"].get<std::string>();
        {//to parse the raw string from json into readable
            size_t pos = 0;
            while ((pos = reply_context.find("\\n", pos)) != std::string::npos) {
                reply_context.replace(pos, 2, "\n");
                pos += 1;
            }
        }
        message_reply=this->profile.name+" : "+reply_context;
        if (agent_reply.contains("tool_calls")){//if there is tool usage
            this->chat_state=2;
            this->handle_tool_request(&agent_reply);
            return (reply_context.empty()?("(calling tool)"+this->profile.name+" : "+".."):"(calling tool)"+message_reply);
        }else this->message_pool.push_back({{"role",agent_reply["role"]},{"content",agent_reply["content"]}});//if no tool,then reply and end this session
    }
    
    else{//if http Response is not Succeed
        this->message_pool.push_back(default_reply);//this Prevents broken message Sequence
        return "LLM error: "+to_string(agent_reply["code_"])+" #type:"+to_string(agent_reply["error_type"])+" #message:"+to_string(agent_reply["error_message"]);
    }
    
    this->working_count=0;
    this->fail_count=0;
    this->chat_state=1;
    return (message_reply.empty()?(this->profile.name+" : "+".."):message_reply);
}

string sagtlib::Agent::load_cfg(const string& a){
    
    {//the Model setup
        json data=handle_read_json(this->home,this->room,"profile.json");//build the home/room/profile.json path and read file
        this->profile.api = (data.contains("APIKEY")?data["APIKEY"]:"");
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
                "\n **who you are**:"+read_as_string(this->home,this->room,"IDENTITY.md")+\
                "\n **General RULE**:"+read_as_string(this->home,"RULE.md")+\
                "\n **your Additional RULE**:"+read_as_string(this->home,this->room,"SUBRULE.md")
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

string sagtlib::Agent::load_cht(const string& option){
    int choice;
    try{
        choice=stoi(option);
    }catch(const exception e){
        choice=-23;
    }
    string I=handle_get_history(this->home,this->room,choice);
    if(I[0]=='\n')return I;
    json data=handle_read_json(this->home,this->room,I);
    if(data.empty())return "cant use this file as reload chat\n";
    // this->message_pool.clear();
    // this->message_pool.shrink_to_fit();sleep_2(3)
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
        return handle_save(&save,this->home,this->room,"profile.json");
    }else return "no such option\n";
}

sagtlib::Agent::Agent(const string& home,const string& room):home(home),room(room){
    {//preconfig
        this->port_num=-1;
        this->socket_num=-1;//will be distributed by system kernel 
        this->start_main_thread();
        for (int i = 0; i < INPUT_POOL_SIZE; ++i)this->input_pool[i] = inputs_{.message="",.image="",.client_socket=0}; 
        this->push_in=0;
        this->push_out=0;
        this->queued_input=0;
    }
    cout<<this->load_cfg("");
}

sagtlib::Agent::~Agent(){
    cout<<this->save("h");
    this->stop_all_thread();
    sleep_2(1)
    cout<<"agent service terminated..\n";
}
