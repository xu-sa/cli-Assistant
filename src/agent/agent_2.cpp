//Thread Management
#include "./sagent.h"
#include <iostream>
#define PRINT_ERROR std::cout<<"Unexpected Error Occurred : 6";
using namespace std;

void sagtlib::Agent::push_input(int i,const string& client_id,const string& text,const string& image){//input port
    sleep_2(2)
    if(client_id==""){
        this->respond_socket("Error: Undefined user id",2,i);
        return;
    }
    if(this->queued_input==10){
        if(i!=-1)this->respond_socket("Error: Server is busy",2,i);
        else cout<<"Cant handle such a huge data stream in once";
        return;
    };
    this->input_pool[this->push_in].client_id=client_id;
    this->input_pool[this->push_in].message=text;
    this->input_pool[this->push_in].image=image;
    this->input_pool[this->push_in].client_socket=i;//client socket number , -1 for local terminal
    this->push_in=(this->push_in+1)%INPUT_POOL_SIZE;
    lock_guard<mutex> locker(this->input_mutex);
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
        lock_guard<mutex> locker(this->input_mutex);
        this->queued_input-=1;
    }
}

void sagtlib::Agent::start_main_thread(){
    this->stop_all_thread();sleep_2(3)
    this->on=true;
    this->main_thread=thread(&sagtlib::Agent::listen_input,this);
    cout<<"main thread is on\n";
}
 
string sagtlib::Agent::start_server_thread(const string& input_port){
    if(this->socket_num!=-1){
        return "please close current server socket before listening on another socket\n";
    }
    int port;
    try{
        port=stoi(input_port);
    }catch(const exception& e){
        port=-1;
    }
    this->port_num=port;
    if(this->port_num==-1){
        return "not a correct port number\n";
    }else
    if(this->on){
        this->start_server();
        if(this->socket_num==-1)return "";
        this->server_thread=thread(&sagtlib::Agent::listen_server,this);
        return "server thread start on port "+to_string(this->port_num)+"\n";
    }
    else return "main thread is not been Activated yet..cant start server now.\n";
    
} 

string sagtlib::Agent::stop_server_thread(const string& a){
    if(this->server_thread.joinable()){
        this->stop_server();
        this->server_thread.join();
        return "server thread shut down..\n";
    }
    else return "server is not been Activated yet..cant join a unjoinable thread\n";
}

void sagtlib::Agent::stop_all_thread(){
    this->on=0;
    if (this->main_thread.joinable()){
        this->main_thread.join();
        std::cout<<"\nagent "<<this->profile.name<<"'s main thread is terminated"<<endl;
    }
    if (this->server_thread.joinable()){
        cout<<this->stop_server_thread("");
    }
    sleep_2(3)
}

