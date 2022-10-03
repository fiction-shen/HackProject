#ifndef _send_keyboarad_h
#define _send_keyboarad_h

int init_socket();

void send_msg(int sock_cli,void *msg,const int size);

#endif