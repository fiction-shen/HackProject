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
#include "keyboard_simulate.h"
#include <thread>
#include <functional>



int main(int argc, char **argv) {
    
    SocketLink server_socket;
    server_socket.init_socket();
    KeyboardBase keyb;

    std::thread t = std::thread(std::mem_fn(&KeyboardBase::ProcMsg), keyb, SocketLink::receive_imu_);
    // std::thread t(&KeyboardBase::ProcMsg, keyb, SocketLink::receive_imu_);
    // keyb.ProcMsg(SocketLink::receive_imu_);
    while(1)
    {
        server_socket.socket_monitor();
        // keyb.ProcMsg(SocketLink::receive_imu_);
    }
   
    return 0;
 }





