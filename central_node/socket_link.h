#ifndef __SOCKET_LINK_H_
#define __SOCKET_LINK_H_

#include <vector>
#include <memory>


class SocketLink
{
private:
    struct pthread_data {
        struct sockaddr_in client_addr;
        int sock_fd;
    };
    /* data */
    std::vector<pthread_t*> threads_;
    struct pthread_data pdata_;

    int server_socket_;
    std::vector<int> new_server_sockets_;
public:
    SocketLink();
    ~SocketLink();

    int init_socket();
    int socket_monitor();

    static void *task_for_client(void *arg);
};







#endif