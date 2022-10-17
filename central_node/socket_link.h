#ifndef __SOCKET_LINK_H_
#define __SOCKET_LINK_H_

#include <vector>
#include <map>
#include <memory>
#include "imu.h"

typedef struct {
    int sock_fd;
    bool state;
    int sensor_type;
}SocketState;

class SocketLink
{
private:
    struct pthread_data {
        struct sockaddr_in client_addr;
        int sock_fd;
    };

    struct pthread_data pdata_;
    int server_socket_;

    
public:
    SocketLink();
    ~SocketLink();

    int init_socket();
    int socket_monitor();

    static void *task_for_client(void *arg);
    static std::shared_ptr<ImuData> receive_imu_;

    /* data */
    static std::vector<std::shared_ptr<SocketState>> new_server_sockets_;
    static std::map<int,std::shared_ptr<ImuData>> sockfd_bind_imu_;
};









#endif