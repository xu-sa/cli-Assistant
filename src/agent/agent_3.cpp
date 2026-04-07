//Interactive functions
#include "sagent.h"
#include "../data/sydata.h"
#include "../utils/file/util_parse_file.h"
#include <iostream>
#define PRINT_ERROR std::cout<<"Unexpected Error Occurred : 7";
using json=nlohmann::json;
using namespace std;

void sagtlib::Agent::terminalsession(bool a){//start a terminal session for chating
    string input;
    if(!a){
        while (this->on)
        {
            sleep_2(10);
            //cout<<"\n>>";
            //getline(cin,input);
            //if(input=="/exit")break;   
        }
    }else{
        while (this->on)
            {
                getline(cin,input);
                this->push_input(-1,input);
                while (this->queued_input!=0)sleep_2(3)
            }   
    }
    cout<<"session ended.\n";
}

void sagtlib::Agent::handle_input(){
    int from=this->input_pool[this->push_out].client_socket;
    int to_send=0;
    if(this->input_pool[this->push_out].message[0] == '/'){//user need to Execute commands
        string command ,value;
        istringstream ss(this->input_pool[this->push_out].message);
        {
            ss>>command;
            ss.seekg(1,ios::cur);//jump over space Between command and value 
            getline(ss,value);
        }
        if(from==-1){
            if(command=="/exit")this->on=0;
            else if(command=="/save")cout<<this->save(value);
            else if(command=="/relop")cout<<this->load_cfg();
            else if(command=="/reloh")cout<<this->load_cht(value);
            else if(command=="/file")cout<<this->attach_file(value);
            else if(command=="/config")cout<<this->config(value);
            else if(command=="/serveron")this->start_server_thread(value);//only for terminal
            else if(command=="/serveroff")this->stop_server_thread();//only for terminal
            else if(command=="/send")to_send=-1;//only for terminal
            else cout<<this->help()<< "Commands below are only Available for this termial:\n"
                                    <<"    /exit                     ——terminate this session and this agent\n"
                                    <<"    /serveron <int>           ——start server on Specified port\n"
                                    <<"    /serveroff                ——stop server\n"
                                    <<"    /send <any>               ——to send message to agent\n";
        }
        else if(from>=4){
            this->respond_socket("",0);//start socket
            if(command=="/save")this->respond_socket(this->save(value),1);
            else if(command=="/relop")this->respond_socket(this->load_cfg(),1);
            else if(command=="/reloh")this->respond_socket(this->load_cht(value),1);
            else if(command=="/file")this->respond_socket(this->attach_file(value),1);
            else if(command=="/config")this->respond_socket(this->config(value),1);
            else this->respond_socket(this->help(),1);
            this->respond_socket("",-1);//close socket
        }
        else PRINT_ERROR
    }
    else if(from>=4)to_send=1;
    else if(from==-1)cout<<"unknown input, please use '/' for Instructions"<<endl;
    else PRINT_ERROR;
    if(to_send!=0){//have a message to send
        if(to_send==-1)this->input_pool[this->push_out].message.erase(0,6);//get rid of the command prompt
        if(this->input_pool[this->push_out].image[0]=='d'){//there is a image attached 
            json content=json::array();
            content.push_back({{"type","text"},{"text",this->input_pool[this->push_out].message}});
            content.push_back({{"type", "image_url"},{ "image_url",{{"url",this->input_pool[this->push_out].image}}}});//data:image/png;base64,iVBO.....
            this->message_pool.push_back({{"role","user"},{"content",content}});
        
        }
        else this->message_pool.push_back({{"role","user"},{"content",this->input_pool[this->push_out].message}});
        if(to_send==1){//send to socket
            this->respond_socket("",0);//start socket
            this->respond_socket(this->send()+"\n",1);
            while (this->chat_state!=1)this->respond_socket(this->send()+"\n",1);
            this->respond_socket("",-1);//close socket
        }
        else{//send to terminal
            cout<<this->send()<<endl;
            while (this->chat_state!=1)cout<<this->send()<<endl;
        }
    }else return;
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
        this->input_pool[this->push_in].image=decode_image(path);
        if(this->input_pool[this->push_in].image[0]=='d')return "add a image '"+path+"'\n";
        else if(this->input_pool[this->push_in].image[0]=='0')return "Detected image path but Cant open(x)\n";
        else if(this->input_pool[this->push_in].image[0]=='1')return "image is too large to add(x)\n";
        else if(this->input_pool[this->push_in].image[0]=='2')return "image file detected but doesn't seem to be in good Format\n";
        else PRINT_ERROR
        this->input_pool[this->push_in].image.clear();
    case DOCUMENT:
        this->input_pool[this->push_in].message+=decode_txt(path);
        return "add a text '"+path+"' \n";
    case AUDIO:
        this->input_pool[this->push_in].message+=decode_audio(path);
        return "add a audio '"+path+"' \n";
    case VIDEO:
        this->input_pool[this->push_in].message+=decode_video(path);
        return "add a video '"+path+"' \n";
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
        case 0:
            this->profile.local_llm_socket=(value_int>0?value_int:-1);
            I="\nCurrent Local LLM socket: "+(value_int>0?"127.0.0.1:"+to_string(value_int):"None")+"\n";
            break;
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
            this->profile.model=0;
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
            I= "Configuration: No Such Option\n";
            I+="Usage:/config [option] [value]\n";
            I+="  0 <int>                      Set local llm Socket (this overrides model/provider config)    Current: "+(this->profile.local_llm_socket==-1?"None":to_string(this->profile.local_llm_socket))+"\n";
            I+="  1 <string>                   Set API key \n";
            I+="  2 <string>                   Set agent description( name:'"+this->profile.name+"' would be added in the front)\n";
            I+="  3 <int>                      Set provider        (leave [value] empty to get hint) Current: "+to_string(this->profile.provider)+"\n";
            I+="  4 <int>                      Set model           (leave [value] empty to get hint) Current: "+to_string(this->profile.model)+"\n";
            I+="  5 <int>                      Set open route model(leave [value] empty to get hint) Current: "+to_string(this->profile.openroute_model)+"\n";
            I+="  6 <int>                      Set max tokens          Current: "+to_string(this->profile.max_tokens)+"\n";
            I+="  7 '0'/'1'                    Set stream              Current: "+to_string(this->profile.stream)+"\n";
            I+="  8 <float>                    Set temperature         Current: "+to_string(this->profile.temperature)+"\n";
            I+="  9 <float>                    Set top_p               Current: "+to_string(this->profile.top_p)+"\n";
            I+=" 10 <int>                      Set max messages        Current: "+to_string(this->profile.max_message)+"\n";
            I+=" 11 'none'/'auto'/'required'   Set tool choice         Current: "+this->profile.tool_choice+"\n";
            break;
    }
    return I;
}

string sagtlib::Agent::help(){
    string I="No Such Command,Available:\n";
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
    if(urls[this->profile.provider][1]=="Local LLM")return "Local LLM model is not defined here, it depends on your setup\n";
    if(urls[this->profile.provider][1]=="OpenRouter")return "Open Route provides models that can be viewed via /config 5\n";
    for(int i=0;i<5;i++)if(!models[this->profile.provider][i].empty())I+="    "+to_string(i)+". "+models[this->profile.provider][i]+"\n";
    return I;
}
