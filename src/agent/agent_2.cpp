//Thread Management
#include "./sagent.h"
#include <iostream>
#define PRINT_ERROR std::cout<<"Unexpected Error Occurred : 6";
using namespace std;

void sagtlib::Agent::push_input(int i,const string& text){//input port
    lock_guard<mutex> locker(this->input_mutex);
    sleep_2(2)
    this->input_pool[this->push_in].message+=text;
    this->input_pool[this->push_in].client_socket=i;//client socket number , -1 for local terminal
    this->push_in=(this->push_in+1)%INPUT_POOL_SIZE;
    this->queued_input+=1;
}

void sagtlib::Agent::listen_input(){//main loop thread
    while(this->on){
        sleep_2(5)
        if(this->queued_input==0)continue;
        lock_guard<mutex> locker(this->input_mutex);
        this->handle_input();//run
        //clean up buffer
        this->input_pool[this->push_out].image.clear();
        this->input_pool[this->push_out].message.clear();
        this->input_pool[this->push_in].client_socket=0;
        this->push_out=(this->push_out+1)%INPUT_POOL_SIZE;
        this->queued_input-=1;
    }
}

void sagtlib::Agent::start_main_thread(){
    this->stop_all_thread();sleep_2(3)
    this->on=true;
    this->main_thread=thread(&sagtlib::Agent::listen_input,this);
    cout<<"main thread is on\n";
}
// void sagtlib::Agent::start_tool_thread(){
//     while (this->on)
//     {   
//         sleep_2(5)
//         if(this->chat_state!=2)continue;
//         lock_guard<mutex> locker(this->tool_mutex);
//         this->handle_tool_request();
//     }
    

// }

void sagtlib::Agent::start_server_thread(const string& input_port){
    if(this->socket_num!=-1){
        std::cout<<"please close current server socket before listening on another socket\n";
        return;
    }
    int port;
    try{
        port=stoi(input_port);
    }catch(const exception& e){
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
        cout<<"server thread start on port "<<this->port_num<<"\n";
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
        cout<<"server thread shut down..\n";
    }
    else cout<<"server is not been Activated yet..cant join a unjoinable thread\n";
}

void sagtlib::Agent::stop_all_thread(){
    this->on=0;
    if (this->main_thread.joinable()){
        this->main_thread.join();
        std::cout<<"\nagent "<<this->profile.name<<"'s main thread is terminated"<<endl;
    }
    if (this->server_thread.joinable()){
        this->stop_server_thread();
    }
    sleep_2(3)
}

