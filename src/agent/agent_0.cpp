#include "sagent.h"
#include "../data/sydata.h"
#include <iostream>
#include "../utils/net/utils_net.h"
#define PRINT_ERROR std::cout<<"Unexpected Error Occurred : 4";
using json=nlohmann::json;
using namespace std;

static vector<int> dr_tool_in_use;
static int tool_size=0;
static json request_body;
static json all_tool;
static string message_reply;
static int tool_state[3]={0};//working failed terminal

static void handle_tool_request(sagtlib::Agent* a,json* agent_reply){
    a->message_pool.push_back({{"role",(*agent_reply)["role"]},{"content",(*agent_reply)["content"]},{"tool_calls",(*agent_reply)["tool_calls"]}});
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
        // auto skill_= a->SKILL_map.find(data["name"]);
        // if(skill_!=a->SKILL_map.end())to_run=&a->SKILLs[skill_->second];
        for(int i=0;i<tool_size;i++){
            if(a->SKILLs[i].definition["function"]["name"]==data["name"]){
                a->message_pool.push_back(a->run_tool(&a->SKILLs[i],&data));
                break;
            }
        }
    }
};

static json parse_definition(const string definition_[][5],size_t amount,vector<string >* parameter_format){
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

string sagtlib::Agent::get_skills(const string* a){
    json all=json::array();
    // unsigned int skill_number= this->SKILLs.size();
    if(!a){
        for(size_t i=0;i<tool_size;i++){
            json def;
            if(this->SKILLs[i].state)def=this->SKILLs[i].definition;
            if(!def.empty() && !def.is_null())all.push_back(def);
        };
    }else{
        for(size_t i=0;i<tool_size;i++){
            json def;
            if(!this->SKILLs[i].state){
                def["tag"]=i;
                def["definition"]=this->SKILLs[i].definition["function"]["description"];
                def["name"]=this->SKILLs[i].definition["function"]["name"];
                all.push_back(def);
            }
        };
    }
    return all.dump();
};

string sagtlib::Agent::activate_tool(const string* a){
    int tag=0;
    try{
        tag=stoi(a[0]);
        if(tag<=2&&tag>=tool_size)return "Please use a valid number";
        this->SKILLs[tag].state=1;
        dr_tool_in_use.push_back(tag);
        return  "Activated DR tool by tag "+a[0]+\
                ". name: "+this->SKILLs[tag].definition["function"]["name"].dump()+\
                "Parameters: "+this->SKILLs[tag].definition["function"]["parameters"].dump();
    }catch(exception& e){
        return "error: "+string(e.what());
    }
}

void sagtlib::Agent::register_tool(const string definition_[][5],size_t amount, std::function<std::string(const std::string*)> fuc) {
    SKILL new_skill;
    json definition = parse_definition(definition_,amount,&new_skill.parameter_format);
    if(definition["type"]=="Wrong")return;
    for(int i=0;i<tool_size;i++){
        if(this->SKILLs[i].definition["function"]["name"]==definition["function"]["name"]){
            this->SKILLs[i].definition=definition;
            this->SKILLs[i].fuc=fuc;
            MES_0_0
            return;
        }
    }
    new_skill.definition=definition;
    new_skill.fuc = fuc;
    new_skill.state=definition_[0][3]=="";
    new_skill.type=definition_[0][2];
    this->SKILLs.push_back(new_skill);
    tool_size++;
    MES_0_1
    return;
}

json sagtlib::Agent::run_tool(SKILL* s,json* data){
    if(!s)return {{"role","tool"},{"content","This tool is not registered/no such tool"},{"tool_call_id",(*data)["id"]}};
    // if(this->debugger)cout<<"\nDEBUGGER: Handling Tool Request Data : "<<(*data).dump()<<endl;
    if(tool_state[0]==TOOL_CALL_LIM)return{{"role","tool"},{"content","You Have Been consistently request tool for too many times,take a break and do a short summarize"},{"tool_call_id",(*data)["id"]}};
    if(tool_state[1]>=TOOL_FAILED_LIM)return{{"role","tool"},{"content","Tool keeps Failing , please report this malfunctioning tool"},{"tool_call_id",(*data)["id"]}};      
    if(tool_state[2]>=TERMINAL_CALL_LIM)return{{"role","tool"},{"content","Terminal tool call ratio Limit Reached,please slow down."},{"tool_call_id",(*data)["id"]}};
    if(!s->state) return {{"role","tool"},{"content","The Tool is temporarily Disabled"},{"tool_call_id",(*data)["id"]}};
    json result;
    result["role"]="tool";
    result["tool_call_id"]=(*data)["id"];
    try{
        tool_state[0]+=1;
        tool_state[2]+=((*data)["name"]=="terminal_"?1:0);
        string data_[5];        
        if(s->type==""){//not a extension skill,has tag within [0,2]
            for(size_t i=0;i<s->parameter_format.size();i++)data_[i]=(
                (*data)["arguments"][s->parameter_format[i]].is_null()?"":
                ((*data)["arguments"][s->parameter_format[i]].is_string()?
                (*data)["arguments"][s->parameter_format[i]].get<std::string>():
                (*data)["arguments"][s->parameter_format[i]].dump()
            ));
        }else{
            data_[0]=s->type;
            data_[1]=(*data)["name"];
            data_[2]=(*data)["arguments"].dump();
            data_[3]=this->profile.extension_env;
        }
        result["content"]=s->fuc(data_);
        
    }
    catch(const std::exception& e){
        result["content"]=std::string(e.what());
        tool_state[1]+=1;
    };
    return result;
}

string sagtlib::Agent::send(){
    message_reply.clear();
    if( this->message_pool.size()>=this->profile.max_message){//clean up the Earliest two message when there is too much chat ,Regardless of System Prompt
        this->message_pool.erase(this->message_pool.begin() + 2);
        this->message_pool.erase(this->message_pool.begin() + 1);
        while(this->message_pool.size()>=2&&this->message_pool[1]["role"]=="tool")this->message_pool.erase(this->message_pool.begin()+1);
    };
    {//to build the request body
        string model=(this->profile.local_llm_socket!=-1?"local llm"
            :(urls[this->profile.provider][1]=="OpenRoute"?models_open_router[this->profile.openroute_model]
            :models[this->profile.provider][this->profile.model]
        ));
        all_tool=json::parse(this->get_skills(NULL));
        request_body["messages"]=this->message_pool;
        request_body["tools"] =all_tool;
        request_body["model"]=model;
        request_body["temperature"]=this->profile.temperature;  
        request_body["top_p"]=this->profile.top_p;
        request_body["max_tokens"]=this->profile.max_tokens;
        request_body["stream"]=this->profile.stream;
        request_body["tool_choice"] =this->profile.tool_choice;
    }
    
    if(this->input_pool[this->push_out].image!=""){//if there is a image base64 string in the message
        string message;
        for (const auto& item : this->message_pool.back()["content"])if (item["type"] == "text")message += item["text"].get<std::string>();
        this->message_pool.back()["content"].clear();
        this->message_pool.back()["content"]=message+"\n**here was a Instant-viewed image**";
    }
    json agent_reply = handle_post((this->profile.local_llm_socket==-1?urls[this->profile.provider][0]:"127.0.0.1:"+to_string(this->profile.local_llm_socket)+"/chat/completions"),this->profile.api,&request_body);
    request_body.clear();
    all_tool.clear();

    if (agent_reply["code_"] == 200) {
        message_reply=agent_reply["content"].get<std::string>();
        {//to parse the raw string from json into readable
            size_t pos = 0;
            while ((pos = message_reply.find("\\n", pos)) != std::string::npos) {
                message_reply.replace(pos, 2, "\n");
                pos += 1;
            }
        }
        if (agent_reply.contains("tool_calls")){//tool is called
            this->chat_state=2;
            handle_tool_request(this,&agent_reply);
            return MES_0_2
        }else this->message_pool.push_back({{"role",agent_reply["role"]},{"content",agent_reply["content"]}});//if no tool,then reply and end this session
    }
    else{//if http Response is not Succeed
        this->message_pool.push_back({{"role","assistant"},{"content","ummm..."}});//this Prevents broken message Sequence
    }
    this->chat_state=1;
    if(dr_tool_in_use.size()>0){
        for(int i:dr_tool_in_use)this->SKILLs[i].state=0;
        dr_tool_in_use.clear();
    }
    tool_state[0]=0;
    tool_state[1]=0;
    tool_state[2]=0;
    if(agent_reply["code_"]!=200){
        return MES_0_3
    }
    else return MES_0_4
}

sagtlib::Agent::Agent(const string& home,const string& room):home(home),room(room){
    {//preconfig
        this->port_num=-1;
        this->socket_num=-1;//will be distributed by system kernel 
        for (int i = 0; i < INPUT_POOL_SIZE; ++i)this->input_pool[i] = agent_input{.message="",.image="",.client_socket=0}; 
        this->push_in=0;
        this->push_out=0;
        this->queued_input=0;
    }
    cout<<this->load_cf();
    this->start_main_thread();
}

sagtlib::Agent::~Agent(){
    cout<<this->save("h");
    this->stop_all_thread();
    sleep_2(1)
    MES_0_6
}

