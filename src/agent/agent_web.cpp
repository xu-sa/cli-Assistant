//server Management
#include "sagent.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <unistd.h>
#include <string>
using json=nlohmann::json;

void sagtlib::Agent::cout_to_web(const std::string& data){
    json to_send;
    to_send["m"]=data;
    to_send["status"]=(data!=""?1:0);
    std::string response_str = to_send.dump();
    std::string http_response="HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Lenth: "+std::to_string(response_str.length())+"\r\n\r\n"+response_str;
    ::send(this->socket_client_1,http_response.c_str(),http_response.length(),0);
};

void sagtlib::Agent::cout_to_web(const std::string& data,int type){
    json to_send;
    to_send["m"]=data;
    to_send["status"]=type;
    std::string response_str = to_send.dump();
    std::string http_response="HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Lenth: "+std::to_string(response_str.length())+"\r\n\r\n"+response_str;
    ::send(this->socket_client_1,http_response.c_str(),http_response.length(),0);
};

void sagtlib::Agent::cout_to_web(const std::string& data,int type,int socket){
    json to_send;
    to_send["m"]=data;
    to_send["status"]=(data!="");
    std::string response_str = to_send.dump();
    std::string http_response="HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Lenth: "+std::to_string(response_str.length())+"\r\n\r\n"+response_str;
    ::send(socket,http_response.c_str(),http_response.length(),0);
}
       
void sagtlib::Agent::start_server(){
    this->socket_num = socket(AF_INET, SOCK_STREAM, 0);
    if (this->socket_num == -1) {
        std::cerr << "Socket creation failed\n";
        return;
    }
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(this->port_num);
    int opt = 1;
    setsockopt(this->socket_num, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (bind(this->socket_num, (sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Binding failed\n";
        close(this->socket_num);
        return ;
    }
    if (listen(this->socket_num, 3) < 0) {
        std::cerr << "Listen failed\n";
        close(this->socket_num);
        return ;
    }
    std::cout << "Listening on port "<<this->port_num<<"\n";
};

void sagtlib::Agent::stop_server(){
    shutdown(this->socket_num, SHUT_RD);
    close(this->socket_num);
    this->socket_num=-1;
}

void sagtlib::Agent::listen_server(){
    char buffer[1024*4]={0};
    int bytes_count;
    int head_end;
    size_t body_content_info;
    std::string buffer_string;
    while (this->on&&this->socket_num!=-1)
    {
        buffer_string.clear();
        sleep_2
        this->socket_client_1=accept(this->socket_num,nullptr,nullptr);//block till a Client Connects   
        if(this->socket_client_1==-1){//check if failed to allocate a socket for Client
            //handle error
            this->cout_to_web("");
            std::cout<<"error request from client\n";
            close(this->socket_client_1);
            continue;
        }
        std::cout<<"got one client connecting to socket "<<this->socket_client_1<<"\n";
        while (1) {//Receive till found head end, then start to check body content
            int n = recv(this->socket_client_1, buffer, sizeof(buffer) - 1, 0);//0,n-1
            if (n <= 0) break;
            buffer[n] = '\0';
            buffer_string.append(buffer, n);
            head_end=buffer_string.find("\r\n\r\n");
            if (head_end==std::string::npos)continue;
            body_content_info = buffer_string.find("Content-Length: ");
            break;
        }  
        if (body_content_info == std::string::npos) {//not found body info in head
            //handle GET request
            this->cout_to_web(this->help());
            //std::cout<<"GET \n";
            close(this->socket_client_1);
            continue;
        }
        {//start to Receive json content
            int i = std::stoul(buffer_string.substr(body_content_info+16));//body content size
            size_t content_got=buffer_string.length()-head_end;
            while (content_got<i)
            {
                int n=recv(this->socket_client_1,buffer,sizeof(buffer)-1,0);
                if(n<=0)break;
                buffer[n]='\0';
                buffer_string.append(buffer,n);
                content_got+=n;
            }
        }
        try{//handle POST request
            json j= json::parse(buffer_string.substr(head_end+4));
            if(!j["m"].empty()){
                this->push_input(1,j["m"]);
                while (this->queued_input!=0)sleep_2           
            }else this->cout_to_web("");
        }
        catch(const std::exception& e){//client Post a unparsable data
            std::cout<<"Unexpected error: "<<e.what()<<"\n";
            this->cout_to_web("");
        }
        
        close(this->socket_client_1);
    }
    std::cout<<"server thread stoped.\n";
}