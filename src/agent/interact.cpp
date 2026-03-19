#include "sagent.h"
#include <iostream>
#include <unistd.h>
#include "../json/json.hpp"
#include "../utils/utils.h"
using json=nlohmann::json;
using namespace std;

void sagtlib::Agent::direct_chat_session(){
    string input;
    while (this->on)
    {
        cout<<"user: ";
        getline(cin,input);
        this->push_input(0,input);
        {
            int i=this->push_out;
            while (this->push_out==i)sleep_2;
        }
    }   
}

void sagtlib::Agent::push_input(int i,const string& text){//input port
    lock_guard<mutex> locker(this->input_mutex);
    sleep_2
    this->input_pool[this->push_in].message+=text;
    this->input_pool[this->push_in].type=(input_from)i;
    this->push_in=(this->push_in==4?0:this->push_in+1);
    this->queued_input+=1;
}

void sagtlib::Agent::listen_input_thread(){//main loop thread
    while(this->on){
        sleep_2
        if(this->queued_input<=0)continue;
        string message;
        string image;
        input_from type;
        {
            lock_guard<mutex> locker(this->input_mutex);
            message = this->input_pool[this->push_out].message;
            image = this->input_pool[this->push_out].image;
            type = this->input_pool[this->push_out].type;
            this->queued_input-=1;
            this->image_attached=!(image=="");
        }
        
        if(message[0] == '/')// if user need to excutem commands
        {
            string command ,value;
            istringstream ss(message);
            ss>>command;
            ss.seekg(1,ios::cur);//jump over space Between command and value 
            getline(ss,value);
            if(command=="/exit")this->on=0;
            else if(command=="/save")this->save(value);
            else if(command=="/reload")this->load();
            else if(command=="/file")this->attach_file(value);
            else if(command=="/config"){
                if(type==CLT_input){
                    cout<<this->config(value);
                }else{
                    this->cout_to_web(this->config(value));
                }
            }
            else {
                if(type==CLT_input){
                    cout<<this->help();
                }else{
                    this->cout_to_web(this->help());
                }
            }
        
        }
        else if(this->profile.chat_state==READY)//check status and send message
        {
            if(!this->image_attached)this->message_pool.push_back({{"role","user"},{"content",message}});
            else{
                json content=json::array();
                content.push_back({{"type","text"},{"text",message}});
                content.push_back({{"type", "image_url"},{ "image_url",{{"url",image}}}});//data:image/png;base64,iVBO.....
                this->message_pool.push_back({{"role","user"},{"content",content}});
            }
            if(type==NET_input){

            }else cout<<this->send()<<endl;
            while (this->profile.chat_state!=READY)cout<<this->send()<<endl;
        }
        {//step to next
            lock_guard<mutex> locker(this->input_mutex);
            sleep_2
            this->input_pool[this->push_out].message.clear();
            this->input_pool[this->push_out].image.clear();   
            this->push_out=(this->push_out==4?0:this->push_out+1);
        }
    }
    cout<<"\n——agent "<<this->profile.name<<" is been deactivated."<<endl;
    this->message_pool.clear();
    this->message_pool.shrink_to_fit();
}

void sagtlib::Agent::attach_file(const std::string& path){
    string suffix="";
    FileCategory type= (FileCategory)check_file_type(path);
    lock_guard<mutex> locker(this->input_mutex);
    switch (type)
    {
    case NOT_SUPPORTED:
        cout<<"this file is not supported\n";
        this->input_pool[this->push_in].message.clear();
        this->input_pool[this->push_in].message+="\n**user tried to send file ```\n"+path+"\n``` but not able to view in the chat**\n";
        break;
    case NO_MARCH:
        cout<<"this file is found but not in the list\n";
        this->input_pool[this->push_in].message.clear();
        this->input_pool[this->push_in].message+="\n**user tried to send file ```\n"+path+"\n``` but not able to view in the chat**\n";
        break;
    case IMAGE:
        cout<<"adding a image\n";
        this->input_pool[this->push_in].image.clear();
        this->input_pool[this->push_in].image+=decode_image(path);
        break;
    case DOCUMENT:
        std::cout << "adding a text file\n";
        this->input_pool[this->push_in].message.clear();
        this->input_pool[this->push_in].message+=decode_txt(path);
        break;
    case AUDIO:
        cout<<"adding a audio\n";
        this->input_pool[this->push_in].message.clear();
        this->input_pool[this->push_in].message+=decode_audio(path);
        break;
    case VIDEO:
        cout<<"adding a video\n";
        this->input_pool[this->push_in].message.clear();
        this->input_pool[this->push_in].message+=decode_video(path);
        break;    
    default:
    
        cout<<"Unexpected file type\n";
        break;
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
    int value_int=set_int(value);
    float value_float=set_float(value);
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
    string I="System: No Such Command,Available:\n";
    I+="    /exit                     ——terminate this session and this agent\n";
    I+="    /reload                   ——reload config for this agent\n";
    I+="    /save 'h'/'p'             ——choose to save history(h) or config profile(p)\n";
    I+="    /config <integer> <value> ——set different parameter According to the <integar> (0~11) as <value>\n";
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


