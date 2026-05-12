// server Management
#include "sagent.h"
#include <iostream>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    #define CLOSE_SOCKET closesocket
    #define SHUTDOWN(s, how) shutdown(s, how)
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>   
    #define CLOSE_SOCKET close;
    #define SHUTDOWN(s, how) shutdown(s, how)
#endif
// #ifdef MSG_NOSIGNAL
// #define CLOSE_FD(fd) sendto(fd,"{\"type\":0}",50,MSG_NOSIGNAL,NULL,0);
// #else
// #define CLOSE_FD(fd) sendto(fd,"{\"type\":0}",50,0,NULL,0);
// #endif
enum {
    CHUNK_MESSAGE,
    CHUNK_ID,
    CHUNK_IMAGE
};
#define PRINT_ERROR std::cout<<"Unexpected Error Occurred : 8";
#define SEND_IN_ONCE(agent,data,socket) agent->respond_socket("",0,socket);agent->respond_socket(data,1,socket);agent->respond_socket("",-1,socket);
#define BUF_SIZE 1024*10
#define BUF_SIZE2 1024*5
using json=nlohmann::json;
static char buffer[BUF_SIZE]={0};
static uint64_t ofs1=0;
static char buffer2[BUF_SIZE2];
static uint64_t ofs2=0;

static void send_tcp_chunk(int fd,const char* data,uint64_t sz){
    char bf1[8]={0};
    char bf2[1250]={0};
    uint64_t offset=0;
    uint64_t sz2=0;
    for(int i=0;i<sizeof(uint64_t);i++){
        bf1[i]=sz>>(8*(sizeof(uint64_t)-i-1));
    }
    #ifdef MSG_NOSIGNAL
    send(fd,bf1,8,MSG_NOSIGNAL);  // Linux
    #else
    send(fd,bf1,8,0); 
    #endif
    while (offset<sz)
    {
        sz2=sz-offset>1240?1240:sz-offset;
        #ifdef MSG_NOSIGNAL
        send(fd,data+offset,sz2,MSG_NOSIGNAL);//loop sending chunks
        #else
        send(fd,bf2,sz2,0);//loop sending chunks
        #endif
        offset+=sz2;
    }

}

static void recv_tcp_chunk(int fd,char* data,uint64_t* sz_read){
    char bf1[8];
    recv(fd,bf1,8,0);
    uint64_t sz=0;
    for (int i = 0; i < 8; i++)
    {
        sz|=bf1[i]<<(8*(7-i));
    }
    if(*sz_read<sz)
    {//Initial value
        *sz_read=snprintf(data,100,"BUFFEREXCEEDED");
        return;
    }
    *sz_read=0;
    while (*sz_read<sz)
    {
        int i =recv(fd,data+(*sz_read),sz-*sz_read,0);
        if(i<=0)break;
        *sz_read+=i;
    }
    data[*sz_read]='\0';
}

static inline int parse_chunk(char* data,std::string* tokens){
    try
    {
        json j = json::parse(data);
        tokens[CHUNK_ID]      =j.value("id", "");
        tokens[CHUNK_MESSAGE]+=j.value("message","");
        tokens[CHUNK_IMAGE]  +=j.value("image","");
        return 1;
    } 
    catch (const std::exception& e) 
    {
        std::cout << "Unexpected error when parsing: " << e.what() << "\n";
        tokens[CHUNK_IMAGE].clear();
        return 0;
    }
}

static void handle_client_in(sagtlib::Agent* agent,int fd){
    std::string res[3];
    try
    {
        ofs1=BUF_SIZE;
        recv_tcp_chunk(fd,buffer,&ofs1);
        // printf("%s\n%d\n\n",buffer,ofs1);
        if(ofs1>0&&parse_chunk(buffer,res))agent->push_input(fd,res[CHUNK_ID],res[CHUNK_MESSAGE],res[CHUNK_IMAGE]);
        else CLOSE_SOCKET(fd);
    }
    catch(const std::exception& e)
    {
        std::cout<<"Failed to parse input: "<<e.what()<<'\n';
        CLOSE_SOCKET(fd);
    }
}

std::string sagtlib::Agent::interupt(){
    std::string s="";
    if(this->input_pool[this->push_out].client_fd>=4)
    {
        this->respond_socket(MES_4_1,2,this->input_pool[this->push_out].client_fd);
        // printf("AAAA\n");
        ofs2=BUF_SIZE2;
        recv_tcp_chunk(this->input_pool[this->push_out].client_fd,buffer2,&ofs2);
        s.append(buffer2,ofs2);
    }else
    switch (this->input_pool[this->push_out].client_fd)
    {
    case -1:
        std::cout<<MES_4_1;
        getline(std::cin,s);
        break;
    default:
        s="N";
        break;
    }
    return s;
}

void sagtlib::Agent::respond_socket(const std::string& data, int type,int client_fd) {
    json v;
    if(type!=0)
    {
        v["message"]=data;
        v["type"]=type;
        std::string d=v.dump();
        send_tcp_chunk(client_fd,d.c_str(),d.length());
    }else
    {
        // CLOSE_FD(client_fd);
        send_tcp_chunk(client_fd,"{\"type\":0}",10);
        #ifdef _WIN32
            SHUTDOWN(client_fd, SD_BOTH);
        #else 
            SHUTDOWN(client_fd,SHUT_RD);
        #endif
        CLOSE_SOCKET(client_fd);
        
    }
};

void sagtlib::Agent::listen_server() {
    while (this->on && this->socket_fd != -1) {
        sleep_2(2);
        int fd_in=accept(this->socket_fd, NULL, NULL);
        if (fd_in == -1)break;
        MES_4_0
        handle_client_in(this,fd_in);
    }
}

void sagtlib::Agent::start_server() {
    #ifdef _WIN32//Optimization for windows
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed\n";
            return;
        }
    #endif
    this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->socket_fd == -1) { 
        std::cerr << "Socket creation failed\n";
        return;
    }

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(this->port_num);

    int opt = 1;
    if (setsockopt(this->socket_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt)) < 0) {
        this->socket_fd=-1;
        std::cerr << "setsockopt failed\n";
    }

    if (bind(this->socket_fd, (sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Binding failed\n";
        this->socket_fd=-1;
        CLOSE_SOCKET(this->socket_fd);
        return;
    }

    if (listen(this->socket_fd, 3) < 0) {
        std::cerr << "Listen failed\n";
        this->socket_fd=-1;
        CLOSE_SOCKET(this->socket_fd);
        return;
    }
    
}

void sagtlib::Agent::stop_server() {
    if (this->socket_fd != -1) {
        #ifdef _WIN32
            SHUTDOWN(this->socket_fd, SD_BOTH);
        #else 
            SHUTDOWN(this->socket_fd,SHUT_RD);
        #endif
        CLOSE_SOCKET(this->socket_fd);
        this->socket_fd = -1;
        std::cout<<"stopped server\n";
    }
}
