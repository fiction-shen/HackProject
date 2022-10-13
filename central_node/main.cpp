#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <unistd.h>
#include <pthread.h>
#include "imu.h"
#include "socket_link.h"




pthread_mutex_t g_mutext;
struct pthread_data {
     struct sockaddr_in client_addr;
     int sock_fd;
};



void *serveForClient(void *arg);


int main(int argc, char **argv) {
    
    SocketLink server_socket;
    server_socket.init_socket();



    while(1)
    {
        server_socket.socket_monitor();
    }
   
    return 0;
 }


void *serveForClient(void *arg){
    int flag_first_run = 1;
    struct pthread_data *pdata = (struct pthread_data*)arg;
    int new_server_socket = pdata->sock_fd;

    int needRecv=73; // 蓝牙一帧数据的长度
    char *buffer=(char*)malloc(needRecv);
    char *buffer_head=(char*)malloc(needRecv);
    char *buffer_complect=(char*)malloc(needRecv);
    memset(buffer_head,0,needRecv);  
    memset(buffer_complect,0,needRecv);

    printf("dong.han 01\n");
    while(1){
         int pos=0;
         int len;
         memset(buffer,0,needRecv);
        

         while(pos < needRecv) {
            len = recv(new_server_socket, buffer+pos, needRecv, MSG_WAITALL);          
            if (len < 0) {
                printf("Server Recieve Data Failed!\n");
                break;
            }
            pos+=len;
         }
        printf("recv over, len=%d\n",len);
        
        // 拆包
        {
            // search帧头 0x11 0xff
            int index = -1;
            for (int i=0; i<needRecv-1; i++) {
                if (buffer[i] == 0x11 && buffer[i+1] == 0xff) {
                    index = i;
                    break;
                }
            }
            if (index == -1) {
                printf("frame error \n");
            } else {
                int tail_length = index;
                int head_length = needRecv - index;

                memcpy(buffer_complect,buffer_head,head_length); //帧头
                memcpy(buffer_complect+head_length,buffer,tail_length); //帧尾

                memset(buffer_head,0,needRecv);
                memcpy(buffer_head,buffer+index,head_length); //存储帧头
            }
        }
        // 解析
        {
            parse_imu((unsigned char*)buffer_complect,len,std::make_shared<ImuData>());
        }

        for(int i=0; i<needRecv; i++) {
            printf("%x ",buffer_complect[i]);
        }    
        printf("\n"); 
    }
    pthread_exit(0);
}






