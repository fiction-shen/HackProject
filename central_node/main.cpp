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
#include <future>



int main(int argc, char **argv) {
    
    SocketLink server_socket;
    server_socket.init_socket();
    KeyboardBase keyb;
    // auto funcimu = std::bind(&KeyboardBase::ProcMsg, keyb, std::placeholders::_1);  //imu处理函数
    // auto funcges = std::bind(&KeyboardBase::ProGst, keyb, std::placeholders::_1);   //gesture处理函数
    // std::future<void> fooimu = std::async(std::launch::async, funcimu, SocketLink::receive_imu_);
    // std::future<void> fooges = std::async(std::launch::async, funcges, SocketLink::receive_gst_);
    std::thread tgst = std::thread(&KeyboardBase::ProGst, &keyb, SocketLink::receive_gst_);
    std::thread timu = std::thread(&KeyboardBase::ProcMsg, &keyb, SocketLink::receive_imu_);
    

    while(1)
    {
        server_socket.socket_monitor();
    }
   
    return 0;
 }





