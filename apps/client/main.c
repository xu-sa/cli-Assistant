#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    #define CLOSE_SOCKET closesocket
    #define SHUTDOWN(s, how) shutdown(s, how)
#else
    #include <sys/socket.h>
    // #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>   
    #define CLOSE_SOCKET close;
    #define SHUTDOWN(s, how) shutdown(s, how)
#endif
#include <cJSON.h>
#define DEFAULT_PORT 9991
#define SZ_1 1024*5
char buffer_1[SZ_1]={0};
char buffer_2[SZ_1]={0};
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
    // printf("\n\n\n%d\n\n\n",sz);
    if(*sz_read<sz)
    {//Initial value
        *sz_read=snprintf(data,100,"BUFFEREXCEEDED");
        return;
    }
    *sz_read=0;
    while (*sz_read<sz)
    {
        int i =recv(fd,data+*sz_read,sz-*sz_read,0);
        if(i<=0)break;
        *sz_read+=i;
    }
    data[*sz_read]='\0';
}

int parse(char* data,char* ms){
    int res=0;
    cJSON *root = cJSON_Parse(data);
    if (root == NULL) {
        return 0;
    }
    cJSON *message = cJSON_GetObjectItem(root, "message");
    if (message&&cJSON_IsString(message)){
        int i = snprintf(ms,SZ_1-1,"%s", message->valuestring);
        ms[i]='\0';
    }

    cJSON *type = cJSON_GetObjectItem(root, "type");
    if (type&&cJSON_IsNumber(type)){
        res=type->valueint;
    }
    cJSON_Delete(root);
    return res;
}

uint64_t build_input(char* id,char* message,char* image,char** output){
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "message", message);
    cJSON_AddStringToObject(root, "id", id);
    if(image)cJSON_AddStringToObject(root, "image", image);
    output[0]=cJSON_Print(root);
    cJSON_Delete(root);
    return strlen(output[0]);
}

int start_client(int port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    connect(fd, (struct sockaddr*)&addr, sizeof(addr));
    return fd;
}
 
int main(int arc,char* arv[]){
    if(arc<=2){
        printf("Usage sample: ./main -serverport -message  \n");
        return 1;
    }
    int port=atoi(arv[1]);
    port=port>100?port:DEFAULT_PORT;
    int fd=start_client(port);
    size_t ofs=0;
    for(int i=2;i<arc;i++){
        memcpy(buffer_1+ofs,arv[i],strlen(arv[i]));//[ofs,ofs+strlen-1]
        ofs+=strlen(arv[i]);
        if(i!=arc-1){
            buffer_1[ofs]=' ';
            ofs++;}
        buffer_1[ofs]='\0';
    }
    char* to_send;
    uint64_t i = build_input("SuperUser",buffer_1,"",&to_send);
    send_tcp_chunk(fd,to_send,i);
    uint64_t sz;
    free(to_send);
    while (1)
    {
        sz=SZ_1;
        recv_tcp_chunk(fd,buffer_2,&sz);
        int tp=parse(buffer_2,buffer_2);
        switch (tp)
        {
        case 2:
            {
                printf("%s\n>>",buffer_2);
                int i =scanf("%[^\n]", buffer_1);
                buffer_1[i]='\0';
                getchar();
                send_tcp_chunk(fd,buffer_1,i);
            }
            break;
        case 1:
            printf("%s\n",buffer_2);
            break;
        case 0:
            {
                close(fd);
                #ifdef _WIN32
                    SHUTDOWN(fd, SD_BOTH);
                #else 
                    SHUTDOWN(fd,SHUT_RD);
                #endif
                return 0;
            }
        default:
            break;
        }
    }
    

}   