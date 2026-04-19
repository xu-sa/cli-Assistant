//Thread Management
#include "./sagent.h"
#include <iostream>
#define PRINT_ERROR std::cout<<"Unexpected Error Occurred : 6";
using namespace std;
#define SEND_IN_ONCE(agent,data,socket) agent->respond_socket("",0,socket);agent->respond_socket(data,1,socket);agent->respond_socket("",-1,socket);

void sagtlib::Agent::push_input_(agent_input* a){
    sleep_2(2)
    if(a->client_id==""){
        SEND_IN_ONCE(this,MES_2_0,a->client_socket)
        return;
    }
    if(this->queued_input==10){
        if(a->client_socket!=-1){SEND_IN_ONCE(this,MES_2_1,a->client_socket)}
        else MES_2_0;
        return;
    };
    lock_guard<mutex> locker(this->input_mutex);
    this->input_pool[this->push_in]=*a;
    this->push_in=(this->push_in+1)%INPUT_POOL_SIZE;
    this->queued_input+=1;
}

void sagtlib::Agent::push_input(int i,const string& client_id,const string& text,const string& image){//input port
    sleep_2(2)
    
    if(client_id==""){
        SEND_IN_ONCE(this,MES_2_0,i)
        return;
    }
    if(this->queued_input==10){
        if(i!=-1){SEND_IN_ONCE(this,MES_2_1,i)}
        else MES_2_0;
        return;
    };
    lock_guard<mutex> locker(this->input_mutex);
    this->input_pool[this->push_in].client_id=client_id;
    this->input_pool[this->push_in].message=text;
    this->input_pool[this->push_in].image=image;
    this->input_pool[this->push_in].client_socket=i;//client socket number , -1 for local terminal
    this->push_in=(this->push_in+1)%INPUT_POOL_SIZE;
    this->queued_input+=1;
}


void sagtlib::Agent::listen_input(){//main loop thread
    while(this->on){
        sleep_2(5)
        if(this->queued_input==0)continue;
        this->handle_input();
        this->input_pool[this->push_out].image.clear();
        this->input_pool[this->push_out].message.clear();
        this->input_pool[this->push_out].client_id="";
        this->input_pool[this->push_out].client_socket=0;
        this->push_out=(this->push_out+1)%INPUT_POOL_SIZE;
        this->queued_input-=1;
    }
}

void sagtlib::Agent::start_main_thread(){
    this->stop_all_thread();sleep_2(3)
    this->on=true;
    this->main_thread=thread(&sagtlib::Agent::listen_input,this);
    MES_2_3
}
 
string sagtlib::Agent::start_server_thread(const string& input_port){
    if(this->socket_num!=-1){
        return MES_2_4
    }
    int port;
    try{
        port=stoi(input_port);
    }catch(const exception& e){
        port=-1;
    }
    this->port_num=port;
    if(this->port_num==-1){
        return MES_2_5
    }else
    if(this->on){
        this->start_server();
        if(this->socket_num==-1)return "";
        this->server_thread=thread(&sagtlib::Agent::listen_server,this);
        return MES_2_6
    }
    else return MES_2_7
    
} 

string sagtlib::Agent::stop_server_thread(const string& a){
    if(this->server_thread.joinable()){
        this->stop_server();
        this->server_thread.join();
        return MES_2_8
    }
    else return MES_2_9
}

void sagtlib::Agent::stop_all_thread(){
    this->on=0;
    if (this->main_thread.joinable()){
        this->main_thread.join();
        MES_2_10
    }
    if (this->server_thread.joinable()){
        std::cout<<this->stop_server_thread("");
    }
    sleep_2(3)
}

