#include <sys/types.h>
#include <sys/socket.h>                         // 包含套接字函数库
#include <stdio.h>
#include <netinet/in.h>                         // 包含AF_INET相关结构
#include <arpa/inet.h>                      // 包含AF_INET相关操作的函数
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <pthread.h>
 
#define MYPORT  6666
#define BUFFER_SIZE 1024

int init_socket()
{
     ///sockfd
     int sock_cli = socket(AF_INET,SOCK_STREAM, 0);
     struct sockaddr_in servaddr;
     memset(&servaddr, 0, sizeof(servaddr));
     servaddr.sin_family = AF_INET;
     servaddr.sin_port = htons(MYPORT);
     servaddr.sin_addr.s_addr = inet_addr("10.97.109.108");
 
     if (connect(sock_cli, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
     {
         perror("connect");
         exit(1);
     }

     return sock_cli;
}


void send_msg(int sock_cli,void *msg,const int size) {
     
     int needSend=size;
     char *buffer=(char*)malloc(needSend);
     memcpy(buffer,msg,needSend);
 
     int pos=0;
     int len=0; 
     while(pos < needSend)
     {
         len=send(sock_cli, buffer+pos, BUFFER_SIZE,MSG_NOSIGNAL);
         if(len <= 0)
         {
             printf("send fail len less 0 \n");
             break;
         }
         pos+=len;
     }

     printf("Send over!!!\n");
     free(buffer);
     return 0;
 }