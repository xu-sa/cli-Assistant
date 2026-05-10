#include "sagent.h"
#include "../data/sydata.h"
#include <iostream>
#include "../utils/net/utils_net.h"
#define PRINT_ERROR std::cout<<"Unexpected Error Occurred : 4";
using json=nlohmann::json;
using namespace std;

static vector<int> tool_in_use;
static int tool_size=0;
static json request_body;
static json all_tool;
static string message_reply;
static int tool_state[2]={0};//working count & failed count 
static json tool_result;

static void handle_tool_request(sagtlib::Agent* a,json* agent_reply){
    a->message_pool.push_back({{"role",(*agent_reply)["role"]},{"content",(*agent_reply)["content"]},{"tool_calls",(*agent_reply)["tool_calls"]}});
    json agent_tool_usage=(*agent_reply)["tool_calls"];
    int tool_call_count = agent_tool_usage.size();
    for(int i=0;i<tool_call_count;i++){
        // std::cout<<"CURRENTTOOLUSEAGE::"<<tool_state[0]<<"\n";
        tool_result.clear();
        int iii=-1;
        for(int ii=0;ii<tool_size;ii++){
            if(a->SKILLs[ii].definition["function"]["name"]==agent_tool_usage[i]["function"]["name"]){
                iii=ii;
                break;
            }
        }
        if(iii!=-1){
            // std::cout<<"Executing function: "<<string(agent_tool_usage[i]["function"]["name"])<<"\n";
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
            tool_result["content"]=a->run_tool(&a->SKILLs[iii],&data);
        }
        else tool_result["content"]="No such tool Available";
        tool_result["tool_call_id"]=agent_tool_usage[i]["id"];
        tool_result["role"]="tool";
        a->message_pool.push_back(tool_result);
    }
};

static json parse_definition(const string definition_[][5],size_t amount,vector<string >* parameter_format){
    json definition;
    try{
        vector<string> required;
        json properties;
        if(amount>1)for(size_t sz=1;sz<amount;sz++){
            if(definition_[sz][0]=="1")required.push_back(definition_[sz][1]);
            properties[definition_[sz][1]]={{"type",definition_[sz][2]},{"description",definition_[sz][3]}};
            if(definition_[sz][4]!=""){
                vector<string> enum_;
                stringstream ss(definition_[sz][4]);
                string choice;
                while (ss >> choice) enum_.push_back(choice);
                properties[definition_[sz][1]]["enum"]=enum_;
            };
            parameter_format->push_back(definition_[sz][1]);    
        }else{
            properties["p1"]={{"type",STRING_PARAMETER},{"description","this is a auto-generated parameter as a place holder which doesnt effect anything"}};
        }
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

static string get_skill(sagtlib::Agent* a){
    json all=json::array();
    for(size_t i=0;i<tool_size;i++){
        json def;
        if(a->SKILLs[i].state)def=a->SKILLs[i].definition;
        if(!def.empty() && !def.is_null())all.push_back(def);
    };
    return all.dump();
};

string sagtlib::Agent::get_plugin(const std::string* a){
    json all=json::array();
    for(size_t i=0;i<tool_size;i++){
        json def;
        if(!this->SKILLs[i].state){
            def["tag"]=i;
            def["definition"]=this->SKILLs[i].definition["function"]["description"];
            def["name"]=this->SKILLs[i].definition["function"]["name"];
            all.push_back(def);
        }
    };
    return all.dump();
};

string sagtlib::Agent::activate_tool(const string* a){
    int tag=0;
    try{
        tag=stoi(a[0]);
        if(tag<=2&&tag>=tool_size)return "Please use a valid number";
        if(this->SKILLs[tag].state==1){
            return "this is a build-in/Activated tool\n";
        }
        this->SKILLs[tag].state=1;
        tool_in_use.push_back(tag);
        return  "Activated plugin tool by tag "+a[0]+\
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

std::string sagtlib::Agent::run_tool(SKILL* s,json* data){
    if(!s->state)return "The Tool is temporarily Disabled";
    std::string res;//user's extra instruction on this function call
    if(tool_state[0]>=TOOL_CALL_LIM)
    {
        res=this->interupt();
        if(res=="N")return "You Have Been consistently request tool for too many times,take a break and do a short summarize";
        tool_state[0]-=3;
        if(!res.empty())res=std::string("**User message: ")+res+"**";
    }
    if(tool_state[1]>=TOOL_FAILED_LIM){
        res=this->interupt();
        if(res=="N")return "Tool keeps Failing , please report this malfunctioning tool";
        tool_state[1]-=2;
    }     
    try{
        string data_[5];     
        if(s->type=="self"){//not a extension skill,has tag within [0,2]
            for(size_t i=0;i<s->parameter_format.size();i++)data_[i]=((*data)["arguments"][s->parameter_format[i]].is_null()?"":
                ((*data)["arguments"][s->parameter_format[i]].is_string()?(*data)["arguments"][s->parameter_format[i]].get<std::string>():
                (*data)["arguments"][s->parameter_format[i]].dump())
            );
        }else{
            data_[0]=s->type;
            data_[1]=(*data)["name"];
            data_[2]=(*data)["arguments"].dump();
            data_[3]=this->profile.extension_env;
        }
        res+=s->fuc(data_);
        tool_state[0]++;
        
    }
    catch(const std::exception& e){
        res+=std::string(e.what());
        tool_state[1]+=1;
    };
    return res;
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
        all_tool=json::parse(get_skill(this));
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
    else this->message_pool.push_back({{"role","assistant"},{"content","...."}});//this Prevents broken message Sequence
    this->chat_state=1;
    if(tool_in_use.size()>0){
        for(int i:tool_in_use)this->SKILLs[i].state=0;
        tool_in_use.clear();
    }
    tool_state[0]=0;
    tool_state[1]=0;
    if(agent_reply["code_"]!=200){
        return MES_0_3
    }
    else return MES_0_4
}

sagtlib::Agent::Agent(const string& home,const string& name):home(home){
    {//preconfig
        this->port_num=-1;
        this->socket_fd=-1;//will be distributed by system kernel 
        for (int i = 0; i < INPUT_POOL_SIZE; ++i)this->input_pool[i] = agent_input{.message="",.image="",.client_fd=0}; 
        this->push_in=0;
        this->push_out=0;
        this->queued_input=0;
    }
    this->profile.name = name;
    cout<<this->load_cf();
    this->start_main_thread();
}

sagtlib::Agent::~Agent(){
    cout<<this->save("h");
    this->stop_all_thread();
    sleep_2(1)
    MES_0_6
}