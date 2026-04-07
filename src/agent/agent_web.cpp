// server Management
#include "sagent.h"
#include <iostream>
//#include <string>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    #define CLOSE_SOCKET closesocket
    #define SHUTDOWN(sock, how) shutdown(sock, how)
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>   
    #define CLOSE_SOCKET close
    #define SHUTDOWN(sock, how) shutdown(sock, how)
#endif
#define PRINT_ERROR std::cout<<"Unexpected Error Occurred : 8";
using json = nlohmann::json;

static void handle_client_in(sagtlib::Agent* agent,int socket){
    char buffer[1024 * 4] = {0};
    size_t body_content_info;
    std::string buffer_string;
    int bytes_count;
    int head_end;
    while (1) {//stat to receive head content
        // recv
        int n = recv(socket, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) break; 
        
        buffer[n] = '\0';
        buffer_string.append(buffer, n);
        head_end = buffer_string.find("\r\n\r\n");
        if (head_end == std::string::npos) continue;
        body_content_info = buffer_string.find("Content-Length: ");
        break;
    }
    
    if (body_content_info == std::string::npos) {// response a GET 
        agent->respond_socket(agent->help(),2);// post a data here
    }
    { // start to Receive json content
        try {
            int i = std::stoul(buffer_string.substr(body_content_info + 16)); 
            size_t content_got = buffer_string.length() - (head_end + 4);
            while (content_got < (size_t)i) {
                int n = recv(socket, buffer, sizeof(buffer) - 1, 0);
                if (n <= 0) break;
                buffer[n] = '\0';
                buffer_string.append(buffer, n);
                content_got += n;
            }
        } catch (...) {
            std::cout<<"Error code 3\n";
            CLOSE_SOCKET(socket);
        }
    }
    
    try{ // handle POST request
        json j = json::parse(buffer_string.substr(head_end + 4));
        if (j["message"].is_string()&&j["message"]!="")agent->push_input(socket, j["message"]);//push in message
        else CLOSE_SOCKET(socket);//unexpected issue
    } catch (const std::exception& e) { // client Post a unparsable data
        std::cout << "Unexpected error: " << e.what() << "\n";
        agent->respond_socket(e.what(),2);// response here
    }

}

inline static void send_chunk(int client_fd, const std::string& data) {
    if (data.empty()) return;
    std::stringstream ss;
    ss << std::hex << data.length() << "\r\n" << data << "\r\n";
    std::string packet = ss.str();
    ::send(client_fd, packet.c_str(), packet.length(), 0);
}

void sagtlib::Agent::respond_socket(const std::string& data, int type) {
    int client_fd = this->input_pool[this->push_out].client_socket;
    switch(type){
        case 0://start stream
            {
                std::string header = 
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: application/json\r\n"
                    "Transfer-Encoding: chunked\r\n"
                    "\r\n";
                ::send(client_fd, header.c_str(), header.length(), 0);
            }
            break;
        case 1://continue sending
            {
                json chunk={};
                chunk["status"]=type;
                size_t data_len = data.length();
                size_t offset = 0;
                while (offset < data_len) {
                    size_t chunk_size = std::min((size_t)512, data_len - offset);
                    std::string data_chunk = data.substr(offset, chunk_size);
                    offset += chunk_size;
                    chunk["message"]=data_chunk;
                    chunk["end"] = (offset >= data_len) ? 1 : 0;
                    send_chunk(client_fd, chunk.dump());
                }
            }
            break;
        case 2://one time response
            {
                json package={};
                package["status"]=type;
                package["message"]=data;
                
                std::string body=package.dump();
                std::string to_send = "POST /path HTTP/1.1\r\n"
                      "Host: example.com\r\n"
                      "Content-Type: application/json\r\n"
                      "Content-Length: " + std::to_string(body.size()) + "\r\n"
                      "\r\n" + body;
                ::send(client_fd,to_send.c_str(),to_send.length(),0);
                CLOSE_SOCKET(client_fd);
            }
            break;
        case -1://end of stream
            ::send(client_fd, "0\r\n\r\n", 5, 0);
            CLOSE_SOCKET(client_fd);
        break;
        default:
            break;
    }
};

void sagtlib::Agent::start_server() {
    #ifdef _WIN32//Optimization for windows
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed\n";
            return;
        }
    #endif
    

    sleep_2(3);
    
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
    if (setsockopt(this->socket_num, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt)) < 0) {
        this->socket_num=-1;
        std::cerr << "setsockopt failed\n";
    }

    if (bind(this->socket_num, (sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Binding failed\n";
        this->socket_num=-1;
        CLOSE_SOCKET(this->socket_num);
        return;
    }

    if (listen(this->socket_num, 3) < 0) {
        std::cerr << "Listen failed\n";
        this->socket_num=-1;
        CLOSE_SOCKET(this->socket_num);
        return;
    }
    
}

void sagtlib::Agent::stop_server() {
    if (this->socket_num != -1) {
        #ifdef _WIN32
            SHUTDOWN(this->socket_num, SD_BOTH);
        #else 
            SHUTDOWN(this->socket_num,SHUT_RD);
        #endif
        CLOSE_SOCKET(this->socket_num);
        this->socket_num = -1;
        std::cout<<"stopped server\n";
    }
}

void sagtlib::Agent::listen_server() {
    while (this->on && this->socket_num != -1) {
        sleep_2(2);
        int socket_in=accept(this->socket_num, nullptr, nullptr);
        if (socket_in == -1)break;
        std::cout << "got one client connecting to socket " << socket_in << "\n";
        handle_client_in(this,socket_in);
    }
}