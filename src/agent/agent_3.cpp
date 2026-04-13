//Interactive functions
#include "sagent.h"
#include "../data/sydata.h"
#include "../utils/file/util_parse_file.h"
#include <iostream>
#define PRINT_ERROR std::cout<<"Unexpected Error Occurred : 7";
#define PRINT_TEST(x) std::cout<<"debugger "<<x<<"\n";
using json=nlohmann::json;
using namespace std; //   /loads <path>             --load skill from select folder\n
const char* help_message_1="\
No Such Command,Available:\n\
    /loadp                    --load profile for this agent\n\
    /loadh <int>              --load chat for this agent\n\
    /loade                    --load extension skills\n\
    /save 'h'/'p'             --choose to save history(h) or agent profile(p)\n\
    /config <int>             --to alter configuration\n\
    /file <path>              --to add a file\n";
const char* help_message_2="\
Commands below are only Available for this termial:\n\
    /exit                     --terminate this session and this agent\n\
    /serveron <int>           --start server on Specified port\n\
    /serveroff                --stop server\n\
    /send <string>            --to send message to agent\n";

static string help_provider(){
    string I;
    for(int i=0;i<PROVIDER_SIZE;i++)if(!urls[i][1].empty())I+="    "+to_string(i)+". "+urls[i][1]+"\n";
    return I;
}

static string help_open_route_model(){
    string I;
    for(int i=0;i<OPEN_ROUTE_SIZE;i++)if(!models_open_router[i].empty())I+="    "+to_string(i)+". "+models_open_router[i]+"\n";
    return I;
}

static string help_model(int i){
    string I;
    if(urls[i][1]=="Local LLM")return "Local LLM model is not defined here, it depends on your setup\n";
    if(urls[i][1]=="OpenRouter")return "Open Route provides models that can be viewed from /config 4 \n";
    for(int ii=0;ii<5;ii++)if(!models[i][ii].empty())I+="    "+to_string(ii)+". "+models[i][ii]+"\n";
    return I;
}

static int handle_command(sagtlib::Agent* a,int socket_){
    string command ,value;
    {
        istringstream ss(a->input_pool[a->push_out].message);
        ss>>command;
        ss.seekg(1,ios::cur);//jump over space Between command and value 
        getline(ss,value);
    }
    if(socket_==-1){
        if(command=="/exit")a->on=0;
        else if(command=="/send"){
            if(value[0]=='@'){
                string filepath;
                value=value.substr(2);
                size_t fileend = value.find_first_of(" ", 1);
                if(fileend==string::npos)fileend=value.length()-1;
                filepath=value.substr(0,fileend);
                cout<<a->attach_file(filepath);//this->input_pool[this->push_out].image handled
                a->input_pool[a->push_out].message=value.substr(fileend+1);
            }else a->input_pool[a->push_out].message=value;
            return -1;
        }//only for terminal
        else if(command=="/serveron")cout<<a->start_server_thread(value);//only for terminal
        else if(command=="/serveroff")cout<<a->stop_server_thread(value);//only for terminal
        else if(command=="/save")cout<<a->save(value);
        else if(command=="/loadp")cout<<a->load_cf();
        else if(command=="/loade")cout<<a->load_ex( );
        else if(command=="/loadh")cout<<a->load_ch(value);
        else if(command=="/config")cout<<a->config(value);
        else cout<<help_message_1<<help_message_2;
    }
    else if(socket_>=4){
        a->respond_socket("",0,socket_);//start socket
        if(command=="/save")a->respond_socket(a->save(value),1,socket_);
        else if(command=="/loadp")a->respond_socket(a->load_cf( ),1,socket_);
        else if(command=="/loadh")a->respond_socket(a->load_ch(value),1,socket_);
        else if(command=="/loade")a->respond_socket(a->load_ex( ),1,socket_);
        else if(command=="/config")a->respond_socket(a->config(value),1,socket_);
        else a->respond_socket(help_message_1,1,socket_);
       
        a->respond_socket("",-1,socket_);//close socket
    }
    else PRINT_ERROR
    return 0;

}

static void handle_send(sagtlib::Agent* a,int to_send){
    if(a->input_pool[a->push_out].image[0]=='d'){//there is a image attached 
            json content=json::array();
            content.push_back({{"type","text"},{"text",a->input_pool[a->push_out].client_id+": "+a->input_pool[a->push_out].message}});
            content.push_back({{"type", "image_url"},{ "image_url",{{"url",a->input_pool[a->push_out].image}}}});//data:image/png;base64,iVBO.....
            a->message_pool.push_back({{"role","user"},{"content",content}});
    }
    else a->message_pool.push_back({{"role","user"},{"content",a->input_pool[a->push_out].client_id+": "+a->input_pool[a->push_out].message}});
    if(to_send==1){//send to socket
        a->respond_socket("",0,a->input_pool[a->push_out].client_socket);//start socket
        a->respond_socket(a->send()+"\n",1,a->input_pool[a->push_out].client_socket);
        while (a->chat_state!=1)a->respond_socket(a->send()+"\n",1,a->input_pool[a->push_out].client_socket);
        a->respond_socket("",-1,a->input_pool[a->push_out].client_socket);//close socket
    }
    else{//send to terminal
        cout<<a->send()<<endl;
        while (a->chat_state!=1)cout<<a->send()<<endl;
    }
}

void sagtlib::Agent::terminalsession(bool a){//start a terminal session for chating
    string input;
    if(!a){
        while (this->on)sleep_2(10);  
    }else{
        while (this->on)
            {
                getline(cin,input);
                this->push_input(-1,LOCAL_USER,input,"");
                while (this->queued_input!=0)sleep_2(3)
            }   
    }
    cout<<"session ended.\n";
}

void sagtlib::Agent::handle_input(){
    int socket_=this->input_pool[this->push_out].client_socket;
    int to_send=0;
    
    if(this->input_pool[this->push_out].message[0] == '/')to_send=handle_command(this,socket_);
    else if(socket_>=4)to_send=1;
    else if(socket_==-1)cout<<"unknown input, please use '/' for Instructions"<<endl;
    else PRINT_ERROR;
    if(to_send==0)return;
   
    handle_send(this,to_send);
}

string sagtlib::Agent::attach_file(const std::string& path){
    string suffix="";
    FileCategory type= check_file_type(path);
    switch (type)
    {
    case NOT_FOUND:
        return "this file '"+path+"' is not available(x)\n";
    case NO_MARCH:
        return "this file '"+path+"' is found but not supported(x)\n";
    case IMAGE:
        this->input_pool[this->push_out].image=decode_image(path);
        if(this->input_pool[this->push_out].image[0]=='d')return "add a image '"+path+"'\n";
        else if(this->input_pool[this->push_out].image[0]=='0')return "Detected image path but Cant open(x)\n";
        else if(this->input_pool[this->push_out].image[0]=='1')return "image is too large to add(x)\n";
        else if(this->input_pool[this->push_out].image[0]=='2')return "image file detected but doesn't seem to be in good Format\n";
        else PRINT_ERROR
        this->input_pool[this->push_out].image.clear();
    case DOCUMENT:
        this->input_pool[this->push_out].message+=decode_txt(path);
        return "add a text '"+path+"' \n";
    case AUDIO:
        this->input_pool[this->push_out].message+=decode_audio(path);
        return "add a audio '"+path+"' \n";
    case VIDEO:
        this->input_pool[this->push_out].message+=decode_video(path);
        return "add a video '"+path+"' \n";
    default:
        return "Unexpected file type\n";
    }    
}

string sagtlib::Agent::config(const string& option){
    int choice,value_int;
    float value_float;
    string I,value;
    {
        istringstream sss(option);
        if(!(sss>>choice))choice=999;
        sss.seekg(1,ios::cur);
        getline(sss,value);
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
        case 0:
            this->profile.local_llm_socket=(value_int>0?value_int:-1);
            I="\nCurrent Local LLM socket: "+(value_int>0?"127.0.0.1:"+to_string(value_int):"None")+"\n";
            break;
        case 1:
            this->profile.api=(value.empty()?this->profile.api:value);
            I="\nCurrent api: "+this->profile.api+"\n";
            break;
        case 2:
            this->profile.provider= (value_int>=0&&value_int<PROVIDER_SIZE?value_int:this->profile.provider);
            I=help_provider();
            this->profile.model=0;
            I+="\nCurrent provider: "+to_string(this->profile.provider)+"\n";
            break;
        case 3:
            this->profile.model=(value_int>=0&&value_int<MODEL_OPTION?value_int:this->profile.model);
            I=help_model(this->profile.provider);
            I+="\nCurrent model: "+to_string(this->profile.model)+"\n";
            break;
        case 4:
            this->profile.openroute_model=(value_int>=0&&value_int<OPEN_ROUTE_SIZE?value_int:this->profile.openroute_model);
            I=help_open_route_model();
            I+="\nCurrent open route model: "+to_string(this->profile.openroute_model)+"\n";
            break;
        case 5:
            this->profile.extension_env=(value.empty()?this->profile.extension_env:value);
            I="\nCurrent Extention path: "+this->profile.extension_env+"\n";
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
            I= "Configuration: No Such Option\n";
            I+="Usage:/config [option] [value]\n";
            I+="  0 <int>                      --Set local LLM Socket (this overrides model/provider config)    Current: "+(this->profile.local_llm_socket==-1?"None":to_string(this->profile.local_llm_socket))+"\n";
            I+="  1 <string>                   --Set API key \n";
            I+="  2 <int>                      --Set LLM provider        (leave [value] empty to get hint)      Current: "+to_string(this->profile.provider)+"\n";
            I+="  3 <int>                      --Set LLM model           (leave [value] empty to get hint)      Current: "+to_string(this->profile.model)+"\n";
            I+="  4 <int>                      --Set OpenRouter LLM      (leave [value] empty to get hint)      Current: "+to_string(this->profile.openroute_model)+"\n";
            I+="  5 <path>                     --Set Extension system path                                      Current: "+this->profile.extension_env+"\n";
            I+="  6 <int>                      --Set max tokens          Current: "+to_string(this->profile.max_tokens)+"\n";
            I+="  7 '0'/'1'                    --Set stream              Current: "+to_string(this->profile.stream)+"\n";
            I+="  8 <float>                    --Set temperature         Current: "+to_string(this->profile.temperature)+"\n";
            I+="  9 <float>                    --Set top_p               Current: "+to_string(this->profile.top_p)+"\n";
            I+=" 10 <int>                      --Set max messages        Current: "+to_string(this->profile.max_message)+"\n";
            I+=" 11 'none'/'auto'/'required'   --Set tool choice         Current: "+this->profile.tool_choice+"\n";
            break;
    }
    return I;
}


