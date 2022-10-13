#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <unistd.h>

#include "socket_link.h"

#define HELLO_WORLD_SERVER_PORT    6666
#define LENGTH_OF_LISTEN_QUEUE     20
#define BUFFER_SIZE                1024




SocketLink::SocketLink(): 
    server_socket_(-1) {
    
}

int SocketLink::init_socket() {

     // set socket's address information
     struct sockaddr_in   server_addr;
     bzero(&server_addr, sizeof(server_addr));
     server_addr.sin_family = AF_INET;
     server_addr.sin_addr.s_addr = htons(INADDR_ANY);
     server_addr.sin_port = htons(HELLO_WORLD_SERVER_PORT);
     
 
     // create a stream socket
     server_socket_ = socket(PF_INET, SOCK_STREAM, 0);
     if (server_socket_ < 0)
     {
         printf("Create Socket Failed!\n");
         return 0;
     }

    //bind
     if (bind(server_socket_, (struct sockaddr*)&server_addr, sizeof(server_addr)))
     {
         printf("Server Bind Port: %d Failed!\n", HELLO_WORLD_SERVER_PORT);
         return 0;
     }
 
     // listen
     if (listen(server_socket_, LENGTH_OF_LISTEN_QUEUE))
     {
         printf("Server Listen Failed!\n");
         return 0;
     }


     return 1;
}



int SocketLink::socket_monitor() {
    struct sockaddr_in client_addr;
    socklen_t          length = sizeof(client_addr);

    int new_server_socket = accept(server_socket_, (struct sockaddr*)&client_addr, &length);
    if (new_server_socket < 0) {
        printf("Server Accept Failed!\n");
        return 0;
    }

    printf("new client connection is build \n");

    new_server_sockets_.push_back(new_server_socket);
    threads_.push_back(new pthread_t());
    struct pthread_data pdata;

    pdata.client_addr = client_addr;
    pdata.sock_fd = new_server_socket;
    pthread_create(threads_[threads_.size()-1],NULL,task_for_client,(void *)&pdata);

    return 1;
}

SocketLink::~SocketLink()
{
    close(server_socket_);
}

static void * SocketLink::task_for_client(void *arg) {


}