#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "gesture.h"
#include "imu.h"
#include "socket_link.h"

#define HELLO_WORLD_SERVER_PORT 6666
#define LENGTH_OF_LISTEN_QUEUE 20
#define BUFFER_SIZE 1024

SocketLink::SocketLink() : server_socket_(-1) {}

int SocketLink::init_socket() {

  // set socket's address information
  struct sockaddr_in server_addr;
  bzero(&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htons(INADDR_ANY);
  server_addr.sin_port = htons(HELLO_WORLD_SERVER_PORT);

  // create a stream socket
  server_socket_ = socket(PF_INET, SOCK_STREAM, 0);
  if (server_socket_ < 0) {
    printf("Create Socket Failed!\n");
    return 0;
  }

  // bind
  if (bind(server_socket_, (struct sockaddr *)&server_addr,
           sizeof(server_addr))) {
    printf("Server Bind Port: %d Failed!\n", HELLO_WORLD_SERVER_PORT);
    return 0;
  }

  // listen
  if (listen(server_socket_, LENGTH_OF_LISTEN_QUEUE)) {
    printf("Server Listen Failed!\n");
    return 0;
  }
  return 1;
}

int SocketLink::socket_monitor() {
  struct sockaddr_in client_addr;
  socklen_t length = sizeof(client_addr);

  int new_server_socket =
      accept(server_socket_, (struct sockaddr *)&client_addr, &length);
  if (new_server_socket < 0) {
    printf("Server Accept Failed!\n");
    return 0;
  }

  printf("new client connection is build \n");

  std::shared_ptr<SocketState> socket_state = std::make_shared<SocketState>();
  socket_state->sock_fd = new_server_socket;
  socket_state->sensor_type = 0; //未知传感器类型
  socket_state->state = 1;       //正常连接
  new_server_sockets_.push_back(socket_state);
  pthread_t thread_id;

  struct pthread_data pdata;
  pdata.client_addr = client_addr;
  pdata.sock_fd = new_server_socket;

  pthread_create(&thread_id, NULL, task_for_client, (void *)&pdata);

  return 1;
}

SocketLink::~SocketLink() { close(server_socket_); }

std::vector<std::shared_ptr<SocketState>> SocketLink::new_server_sockets_;
std::map<int, std::shared_ptr<ImuData>> SocketLink::sockfd_bind_imu_;
std::shared_ptr<ImuData> SocketLink::receive_imu_ = std::make_shared<ImuData>();
std::shared_ptr<GstData> SocketLink::receive_gst_ = std::make_shared<GstData>();
std::shared_ptr<PstData> SocketLink::receive_pst_ = std::make_shared<PstData>();
void *SocketLink::task_for_client(void *arg) {
  struct pthread_data *pdata = (struct pthread_data *)arg;
  int new_server_socket = pdata->sock_fd;

  int needRecv = 73; // 蓝牙一帧数据的长度
  char *buffer = (char *)malloc(needRecv);
  char *buffer_head = (char *)malloc(needRecv);
  char *buffer_complect = (char *)malloc(needRecv);
  int *gstmsg_ = (int *)malloc(sizeof(int));
  int *pstmsg_ = (int *)malloc(sizeof(int));
  memset(buffer_head, 0, needRecv);
  memset(buffer_complect, 0, needRecv);

  while (1) {
    int pos = 0;
    int len;
    memset(buffer, 0, needRecv);
    while (pos < needRecv) {
      len = recv(new_server_socket, buffer + pos, needRecv, MSG_WAITALL);
      if (len < 0) {
        printf("Server Recieve Data Failed!\n");
        break;
      }
      pos += len;
    }

    // for (int i = 0; i < needRecv; i++) {
    //   printf("%x ", buffer[i]);
    // }
    // printf("\n");
    printf("recv over, len=%d\n", len);

    // 拆包

    // search帧头 0x11 0xff
    int index = -1;
    int sensor_type = -1; // 1: imu; 2: gesture

    for (int i = 0; i < needRecv - 1; i++) {
      if ((unsigned char)buffer[i] == 0x11 &&
          (unsigned char)buffer[i + 1] == 0xff) {
        index = i;
        sensor_type = 1;
        break;
      } else if ((unsigned char)buffer[i] == 0x22 &&
                 (unsigned char)buffer[i + 1] == 0xff) {
        index = i;
        sensor_type = 2;
        break;
      } else if ((unsigned char)buffer[i] == 0x33 &&
                 (unsigned char)buffer[i + 1] == 0xff) {
        index = i;
        sensor_type = 3;
        break;
      }
    }

    if (index == -1) {
      printf("frame error \n");
    } else {
      if (sensor_type == 1) { // imu
        int tail_length = index;
        int head_length = needRecv - index;

        memcpy(buffer_complect, buffer_head, head_length);          //帧头
        memcpy(buffer_complect + head_length, buffer, tail_length); //帧尾

        memset(buffer_head, 0, needRecv);
        memcpy(buffer_head, buffer + index, head_length); //存储帧头

        int index = -1;
        for (int i = 0; i < new_server_sockets_.size(); i++) {
          if (new_server_sockets_[i]->sock_fd == new_server_socket) {
            index = i;
            break;
          }
        }
        if (index == -1) {
          printf("thread error \n");
        }

        // 解析
        // std::shared_ptr<ImuData> receive_imu = std::make_shared<ImuData>();
        parse_imu((unsigned char *)buffer_complect, len, receive_imu_);
        // if (sockfd_bind_imu_.find(new_server_socket) ==
        // sockfd_bind_imu_.end()) {

        // }
        // sockfd_bind_imu_[new_server_socket] = receive_imu_;

      } else if (sensor_type == 2) { //手势

        memcpy(gstmsg_, buffer + 2, sizeof(int)); //帧头

        parse_gst(gstmsg_, receive_gst_);
        printf("recived gesture:%d", receive_gst_->gstcode);
      } else if (sensor_type == 3) {  //姿态

        memcpy(pstmsg_, buffer + 2, sizeof(int));  //帧头

        parse_pst(pstmsg_, receive_pst_);
        printf("recived gesture:%d", receive_pst_->pstcode);
      }
    }

    // for (int i = 0; i < needRecv; i++) {
    //   printf("%x ", buffer_complect[i]);
    // }
    printf("\n");
  }

  printf("thread normal exit \n");
  pthread_exit(0);
}