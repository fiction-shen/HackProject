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



int main(int argc, char **argv) {
    
    SocketLink server_socket;
    server_socket.init_socket();



    while(1)
    {
        server_socket.socket_monitor();
    }
   
    return 0;
 }





