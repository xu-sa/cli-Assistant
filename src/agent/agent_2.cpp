//Thread Management
#include "./sagent.h"
#include <iostream>
#include <unistd.h>
#define PRINT_ERROR std::cout<<"Unexpected Error Occurred : 2";
using namespace std;

void sagtlib::Agent::push_input(int i,const string& text){//input port
    lock_guard<mutex> locker(this->input_mutex);
    sleep_2
    this->input_pool[this->push_in].message+=text;
    this->input_pool[this->push_in].type=(input_from)i;
    this->push_in=(this->push_in==4?0:this->push_in+1);
    this->queued_input+=1;
}

void sagtlib::Agent::start_main_thread(){
    this->stop_all_thread();sleep_2
    this->on=true;
    this->main_thread=thread(&sagtlib::Agent::listen_input,this);
    cout<<"main thread is on\n";
}

void sagtlib::Agent::start_server_thread(string& input_port){
    int port;
    try{
        port=stoi(input_port);
    }catch(const exception e){
        port=-1;
    }
    this->port_num=port;
    if(this->port_num==-1){
        cout<<"not a correct port number\n";
        return;
    }else
    if(this->on){
        this->start_server();
        if(this->socket_num==-1)return;
        this->server_thread=thread(&sagtlib::Agent::listen_server,this);
        cout<<"server thread is on\n";
    }
    else {
        cout<<"main thread is not been Activated yet..cant start server now.\n";
        return;
    }
    
} 

void sagtlib::Agent::stop_server_thread(){
    if(this->server_thread.joinable()){
        this->stop_server();
        this->server_thread.join();
    }
    else cout<<"server is not been Activated yet..cant join a unjoinable thread\n";
}

void sagtlib::Agent::stop_all_thread(){
    this->on = false;
    if (this->main_thread.joinable())this->main_thread.join();
    if (this->server_thread.joinable())this->server_thread.join();
    sleep_2
}

void sagtlib::Agent::listen_input(){//main loop thread
    while(this->on){
        sleep_2
        if(this->queued_input<=0)continue;
        lock_guard<mutex> locker(this->input_mutex);        
        if(this->input_pool[this->push_out].message[0] == '/')// if user need to Execute commands
        {
            string command ,value;
            istringstream ss(this->input_pool[this->push_out].message);
            ss>>command;
            ss.seekg(1,ios::cur);//jump over space Between command and value 
            getline(ss,value);
            if(this->input_pool[this->push_out].type==CLI_input){
                if(command=="/exit")this->on=0;
                else if(command=="/save")cout<<this->save(value);
                else if(command=="/relop")cout<<this->load_cfg();
                else if(command=="/reloh")cout<<this->load_cht(value);
                else if(command=="/file")cout<<this->attach_file(value);
                else if(command=="/config")cout<<this->config(value);
                else if(command=="/serveron")this->start_server_thread(value);//only for terminal
                else if(command=="/serveroff")this->stop_server_thread();//only for terminal
                else if(command=="/send")this->send_message();//only for terminal
                else cout<<this->help()<<"Commands below are only Available for this termial:\n"
                                       <<"    /serveron <int>           ——start server on Specified port\n"
                                       <<"    /serveroff                ——stop server\n"
                                       <<"    /send <any>               ——to send message to agent\n";
            }else if(this->input_pool[this->push_out].type==NET_input){
                if(command=="/exit")this->on=0;
                else if(command=="/save")this->cout_to_web(this->save(value));
                else if(command=="/relop")this->cout_to_web(this->load_cfg());
                else if(command=="/reloh")this->cout_to_web(this->load_cht(value));
                else if(command=="/file")this->cout_to_web(this->attach_file(value));
                else if(command=="/config")this->cout_to_web(this->config(value));
                else this->cout_to_web(this->help());
            }else PRINT_ERROR
        }
        else if(this->input_pool[this->push_out].type==NET_input)this->send_message();
        else cout<<"unknown input, please use '/' for Instructions\n";
        //clean up buffer
        sleep_2
        this->input_pool[this->push_out].message.clear();
        this->input_pool[this->push_out].image.clear();
        this->push_out=(this->push_out==4?0:this->push_out+1);
        this->queued_input-=1;
        this->image_attached=!(this->input_pool[this->push_out].image=="");
    }
    cout<<"\nagent "<<this->profile.name<<" is been Suspended."<<endl;
}
