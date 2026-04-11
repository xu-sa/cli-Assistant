#include "sagent.h"
#include "../data/sydata.h"
#include <iostream>
#include "../utils/net/utils_net.h"
#define PRINT_ERROR std::cout<<"Unexpected Error Occurred : 4";
using json=nlohmann::json;
using namespace std;

static void handle_tool_request(sagtlib::Agent* a,json* agent_reply){
    a->message_pool.push_back({{"role",(*agent_reply)["role"]},{"content",(*agent_reply)["content"]},{"tool_calls",(*agent_reply)["tool_calls"]}});
    json agent_tool_usage=(*agent_reply)["tool_calls"];
    int tool_call_count = agent_tool_usage.size();
    for(int i=0;i<tool_call_count;i++){
        SKILL* to_run=NULL;
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
        auto skill_= a->SKILL_map.find(data["name"]);
        if(skill_!=a->SKILL_map.end())to_run=&a->SKILLs[skill_->second];
        a->message_pool.push_back(a->run_tool(to_run,&data));
    }
};

static json get_skills(sagtlib::Agent* a){
    json all=json::array();
    unsigned int skill_number= a->SKILLs.size();
    for(size_t i=0;i<skill_number;i++){
        json def;
        if(a->SKILLs[i].state)def=a->SKILLs[i].definition;
        if(!def.empty() && !def.is_null())all.push_back(def);
    };
    return all;
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

void sagtlib::Agent::register_tool(const string definition_[][5],size_t amount, std::function<std::string(const std::string*)> Actual_tool) {
    SKILL new_skill;
    json definition = parse_definition(definition_,amount,&new_skill.parameter_format);
    if(definition["type"]=="Wrong")return;
    auto skill_= this->SKILL_map.find(definition["function"]["name"]);
    if(skill_!=this->SKILL_map.end()){
        this->SKILLs[skill_->second].definition=definition;
        this->SKILLs[skill_->second].Actual_tool=Actual_tool;
        cout<<"updated skill "<<definition["function"]["name"]<<"\n";
        return;
    }
    new_skill.definition=definition;
    new_skill.Actual_tool = Actual_tool;
    new_skill.state=true;
    this->SKILLs.push_back(new_skill);
    this->SKILL_map[definition["function"]["name"]]=this->SKILLs.size()-1;
    cout<<"add skill "<<definition["function"]["name"]<<"\n";
    return;
}

json sagtlib::Agent::run_tool(SKILL* s,json* data){
    if(!s)return {{"role","tool"},{"content","This tool is not registered/no such tool"},{"tool_call_id",(*data)["id"]}};
    if(this->debugger)cout<<"\nDEBUGGER: Handling Tool Request Data : "<<(*data).dump()<<endl;
    if(this->working_count==6)return{{"role","tool"},{"content","You Have Been Continually Request for Tool for too many times,report to user the situation and ask for next move Instantly"},{"tool_call_id",(*data)["id"]}};
    if(this->fail_count>=4)return{{"role","tool"},{"content","Tool keeps Failing , you may choose to report to user and ask for next step"},{"tool_call_id",(*data)["id"]}};      
    if(!s->state) return {{"role","tool"},{"content","The Tool is temporarily Disabled"},{"tool_call_id",(*data)["id"]}};
    json result;
    result["role"]="tool";
    result["tool_call_id"]=(*data)["id"];
    try{
        size_t pnum=s->parameter_format.size(); 
        string data_[pnum];
        for(size_t i=0;i<pnum;i++)data_[i]=(
            (*data)["arguments"][s->parameter_format[i]].is_null()?"":
            (*data)["arguments"][s->parameter_format[i]].get<std::string>()
        );
        if(this->debugger){
            cout<<"\nDEBUGGER: using tool : "<<s->definition["function"]["name"]<<"\nParameters : ";
            for (const auto& str : data_) {
                std::cout << str << "; ";
            };
        }
        {//get result from tool and build the json
            result["content"]=s->Actual_tool(data_);
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

string sagtlib::Agent::send(){
    string message_reply;
    json request_body;
    if( this->message_pool.size()>=this->profile.max_message){//clean up the Earliest two message when there is too much chat ,Regardless of System Prompt
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
        request_body["tools"] = get_skills(this);
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
            handle_tool_request(this,&agent_reply);
            return (reply_context.empty()?("(calling tool)"+this->profile.name+" : "+".."):"(calling tool)"+message_reply);
        }else this->message_pool.push_back({{"role",agent_reply["role"]},{"content",agent_reply["content"]}});//if no tool,then reply and end this session
    }
    
    else{//if http Response is not Succeed
        this->message_pool.push_back({{"role","assistant"},{"content","ummm..."}});//this Prevents broken message Sequence
        return "LLM error: "+to_string(agent_reply["code_"])+" #type:"+to_string(agent_reply["error_type"])+" #message:"+to_string(agent_reply["error_message"]);
    }
    
    this->working_count=0;
    this->fail_count=0;
    this->chat_state=1;
    return (message_reply.empty()?(this->profile.name+" : "+".."):message_reply);
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
    cout<<this->load_cf();
    if(this->pyskill_env.empty())return;
    cout<<this->load_ex(this->pyskill_env);
}

sagtlib::Agent::~Agent(){
    cout<<this->save("h");
    this->stop_all_thread();
    sleep_2(1)
    cout<<"agent service terminated..\n";
}

