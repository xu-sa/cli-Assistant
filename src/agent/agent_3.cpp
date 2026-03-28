//Interactive functions
#include "sagent.h"
#include "../data/sydata.h"
#include "../utils/file/util_parse_file.h"
#include <iostream>
#define PRINT_ERROR std::cout<<"Unexpected Error Occurred : 2";

using json=nlohmann::json;
using namespace std;

void sagtlib::Agent::terminalsession(bool a){//start a terminal session for chating
    if(!a){
        while (this->on)
        {
            sleep_2(20);
        }
        return;
    }else{
        string input;
        while (this->on)
            {
                getline(cin,input);
                this->push_input(0,input);
                while (this->queued_input!=0)sleep_2(3)
            }   
    }
    cout<<"session ended.\n";
}

void sagtlib::Agent::send_message(){
    if(this->input_pool[this->push_out].message[0]=='/')this->input_pool[this->push_out].message.erase(0,6);
    if(!this->image_attached)this->message_pool.push_back({{"role","user"},{"content",this->input_pool[this->push_out].message}});
    else{
        json content=json::array();
        content.push_back({{"type","text"},{"text",this->input_pool[this->push_out].message}});
        content.push_back({{"type", "image_url"},{ "image_url",{{"url",this->input_pool[this->push_out].image}}}});//data:image/png;base64,iVBO.....
        this->message_pool.push_back({{"role","user"},{"content",content}});
    }
    if(this->input_pool[this->push_out].type==NET_input){
        this->cout_to_web(this->send()+"\n");
        while (this->chat_state!=1)this->cout_to_web(this->send()+"\n");
    }else {
        cout<<this->send()<<endl;
        while (this->chat_state!=1)cout<<this->send()<<endl;
    }
}

string sagtlib::Agent::attach_file(const std::string& path){
    string suffix="";
    FileCategory type= check_file_type(path);
    switch (type)
    {
    case NOT_SUPPORTED:
        this->input_pool[this->push_in].message.clear();
        this->input_pool[this->push_in].message+="\n**user tried to send file ```\n"+path+"\n``` which does not exist**\n";
        return "this file "+path+" is not found\n";
    case NO_MARCH:
        this->input_pool[this->push_in].message.clear();
        this->input_pool[this->push_in].message+="\n**user tried to send file ```\n"+path+"\n``` which cant be view neither from the chat nor via a fetch tool**\n";
        return "this file "+path+" is found but not in the list of supported files\n";
    case IMAGE:
        this->input_pool[this->push_in].image.clear();
        this->input_pool[this->push_in].image+=decode_image(path);
        return "add a image "+path+"\n";
    case DOCUMENT:
        this->input_pool[this->push_in].message.clear();
        this->input_pool[this->push_in].message+=decode_txt(path);
        return "add a text "+path+" \n";
    case AUDIO:
        this->input_pool[this->push_in].message.clear();
        this->input_pool[this->push_in].message+=decode_audio(path);
        return "add a audio "+path+" \n";
    case VIDEO:
        this->input_pool[this->push_in].message.clear();
        this->input_pool[this->push_in].message+=decode_video(path);
        return "add a video "+path+" \n";
    default:
        return "Unexpected file type\n";
    }    
}

string sagtlib::Agent::config(const string& option){
    string I;
    istringstream sss(option);
    int choice;
    string value;
    if(!(sss>>choice)){
        choice=999;
    }
    sss.seekg(1,ios::cur);
    getline(sss,value);
    int value_int;
    float value_float;
    {
        try{
            value_int=stoi(value);
        }catch(const exception& e){
            value_int=-23;
        }
        stringstream ss(value);
        ss>>value_float;
        if (ss.fail() || !ss.eof()) {
            value_float=-24.0f;
        } 
    }
    
    switch(choice){
        case 1:
            this->profile.api=(value.empty()?this->profile.api:value);
            I="\nCurrent api: "+this->profile.api+"\n";
            break;
        case 2:
            this->profile.whoyouare=(value.empty()?this->profile.whoyouare:value);
            I="\nCurrent Discription: "+this->profile.whoyouare+"\n";
            break;
        case 3:
            this->profile.provider= (value_int>=0&&value_int<PROVIDER_SIZE?value_int:this->profile.provider);
            I=this->help_provider();
            I+="\nCurrent provider: "+to_string(this->profile.provider)+"\n";
            break;
        case 4:
            this->profile.model=(value_int>=0&&value_int<MODEL_OPTION?value_int:this->profile.model);
            I=this->help_model();
            I+="\nCurrent model: "+to_string(this->profile.model)+"\n";
            break;
        case 5:
            this->profile.openroute_model=(value_int>=0&&value_int<OPEN_ROUTE_SIZE?value_int:this->profile.openroute_model);
            I=this->help_open_route_model();
            I+="\nCurrent open route model: "+to_string(this->profile.openroute_model)+"\n";
            break;
        case 6:
            this->profile.max_tokens=(value_int>5000||value_int<400?this->profile.max_tokens:value_int);
            I="\nCurrent max tokens: "+to_string(this->profile.max_tokens)+"\n";
            break;
        case 7:
            this->profile.stream=(value=="true"||value=="1");
            I="\nCurrent stream: "+to_string(this->profile.stream)+"\n";
            break;
        case 8:
            this->profile.temperature=(value_float==-24.0f||value_float>2?this->profile.temperature:value_float);
            I="\nCurrent temprature: "+to_string(this->profile.temperature)+"\n";
            break;
        case 9:
            this->profile.top_p=(value_float==-24.0f||value_float>2?this->profile.top_p:value_float);
            I="\nCurrent top p: "+to_string(this->profile.top_p)+"\n";
            break;
        case 10:
            this->profile.max_message=(value_int>150||value_int<10?this->profile.max_message:value_int);
            I="\nCurrent max messages: "+to_string(this->profile.max_message)+"\n";
            break;
        case 11:
            this->profile.tool_choice=(value=="none"||value=="auto"||value=="required"?value:this->profile.tool_choice);
            I="\nCurrent tool choice: "+this->profile.tool_choice+"\n";
            break;
        default:
            I="Configuration: No Such Option\n";
            I+="Usage: [option] [value]\n";
            I+="  1 <string>                   Set API key \n";
            I+="  2 <string>                   Set agent description( name:'"+this->profile.name+"' would be added in the front)\n";
            I+="  3 <int>                      Set provider        (leave [value] empty to get hint) Current: "+to_string(this->profile.provider)+"\n";
            I+="  4 <int>                      Set model           (leave [value] empty to get hint) Current: "+to_string(this->profile.model)+"\n";
            I+="  5 <int>                      Set open route model(leave [value] empty to get hint) Current: "+to_string(this->profile.openroute_model)+"\n";
            I+="  6 <int>                      Set max tokens     Current: "+to_string(this->profile.max_tokens)+"\n";
            I+="  7 '0'/'1'                    Set stream         Current: "+to_string(this->profile.stream)+"\n";
            I+="  8 <float>                    Set temperature    Current: "+to_string(this->profile.temperature)+"\n";
            I+="  9 <float>                    Set top_p          Current: "+to_string(this->profile.top_p)+"\n";
            I+=" 10 <int>                      Set max messages   Current: "+to_string(this->profile.max_message)+"\n";
            I+=" 11 'none'/'auto'/'required'   Set tool choice    Current: "+this->profile.tool_choice+"\n";
            break;
    }
    return I;
}

string sagtlib::Agent::help(){
    string I="No Such Command,Available:\n";
    I+="    /exit                     ——terminate this session and this agent\n";
    I+="    /relop                    ——reload profile for this agent\n";
    I+="    /reloh                    ——reload chat for this agent\n";
    I+="    /save 'h'/'p'             ——choose to save history(h) or config profile(p)\n";
    I+="    /config <int> <value>     ——set different profile parameter\n";
    I+="    /file <path>              ——To insert a local file in the chat to send, will only apply to the next sending session\n";
    return I;
}

string sagtlib::Agent::help_provider(){
    string I;
    for(int i=0;i<PROVIDER_SIZE;i++)if(!urls[i][1].empty())I+="    "+to_string(i)+". "+urls[i][1]+"\n";
    return I;
}

string sagtlib::Agent::help_open_route_model(){
    string I;
    for(int i=0;i<OPEN_ROUTE_SIZE;i++)if(!models_open_router[i].empty())I+="    "+to_string(i)+". "+models_open_router[i]+"\n";
    return I;
}

string sagtlib::Agent::help_model(){
    string I;
    for(int i=0;i<5;i++)if(!models[this->profile.provider][i].empty())I+="    "+to_string(i)+". "+models[this->profile.provider][i]+"\n";
    return I;
}
