#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <unistd.h>
#include <pthread.h>
#include "StaticValue.h"
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include <thread>
 #include <unistd.h>
 #include <vector>

void ClickKey()
{
    // sleep(10);
    while(1) {
    // std::cout << "-----------" << std::endl;
    std::vector<bool> input = InputValue::Getinstance()->GetValue();
    Display* p_display = XOpenDisplay( NULL );
    if(input[0]){
        KeySym keysym = XK_W;
        KeyCode keycode = NoSymbol;
        keycode = XKeysymToKeycode( p_display , keysym );

        XTestFakeKeyEvent( p_display , keycode , True  , 0 ); // 键盘按下"x"
        XFlush( p_display );
        XTestFakeKeyEvent( p_display , keycode , False , 0 ); // 键盘释放"x"
        XFlush( p_display );
    }
    if(input[1]){
        KeySym keysym = XK_S;
        KeyCode keycode = NoSymbol;
        keycode = XKeysymToKeycode( p_display , keysym );

        XTestFakeKeyEvent( p_display , keycode , True  , 0 ); // 键盘按下"x"
        XFlush( p_display );
        XTestFakeKeyEvent( p_display , keycode , False , 0 ); // 键盘释放"x"
        XFlush( p_display );
    }
    if(input[2]){
        KeySym keysym = XK_A;
        KeyCode keycode = NoSymbol;
        keycode = XKeysymToKeycode( p_display , keysym );

        XTestFakeKeyEvent( p_display , keycode , True  , 0 ); // 键盘按下"x"
        XFlush( p_display );
        XTestFakeKeyEvent( p_display , keycode , False , 0 ); // 键盘释放"x"
        XFlush( p_display );
    }
    if(input[3]){
    // if(1){
        KeySym keysym = XK_D;
        KeyCode keycode = NoSymbol;
        keycode = XKeysymToKeycode( p_display , keysym );

        XTestFakeKeyEvent( p_display , keycode , True  , 0 ); // 键盘按下"x"
        XFlush( p_display );
        XTestFakeKeyEvent( p_display , keycode , False , 0 ); // 键盘释放"x"
        XFlush( p_display );
    }
    
    // KeySym keysym = XK_Z;
    // KeyCode keycode = NoSymbol;
    // keycode = XKeysymToKeycode( p_display , keysym );

    // XTestFakeKeyEvent( p_display , keycode , True  , 0 ); // 键盘按下"x"
    // XTestFakeKeyEvent( p_display , keycode , False , 0 ); // 键盘释放"x"
    // XFlush( p_display );

    XCloseDisplay( p_display );
    usleep(300000);
    }
        
    return;
}

#define HELLO_WORLD_SERVER_PORT    6666
#define LENGTH_OF_LISTEN_QUEUE     20
#define BUFFER_SIZE                1024

typedef struct 
{
    bool up;
    bool down;
    bool left;
    bool right;
    bool mouse_left;
    bool mouse_right;
}Keyboard;

pthread_mutex_t g_mutext;
struct pthread_data{
     struct sockaddr_in client_addr;
     int sock_fd;
};

Keyboard *keyboard = (Keyboard *)malloc(sizeof(Keyboard));

void *serveForClient(void *arg);

int main(int argc, char **argv) {
     // set socket's address information
     struct sockaddr_in   server_addr;
     bzero(&server_addr, sizeof(server_addr));
     server_addr.sin_family = AF_INET;
     server_addr.sin_addr.s_addr = htons(INADDR_ANY);
     server_addr.sin_port = htons(HELLO_WORLD_SERVER_PORT);
     pthread_t pt;
     struct pthread_data pdata;

     std::thread keyb(ClickKey);

     // create a stream socket
     int server_socket = socket(PF_INET, SOCK_STREAM, 0);
     if (server_socket < 0)
     {
         printf("Create Socket Failed!\n");
         exit(1);
     }

    //bind
     if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)))
     {
         printf("Server Bind Port: %d Failed!\n", HELLO_WORLD_SERVER_PORT);
         exit(1);
     }

     // listen
     if (listen(server_socket, LENGTH_OF_LISTEN_QUEUE))
     {
         printf("Server Listen Failed!\n");
         exit(1);
     }
     
     while(1)
     {
         struct sockaddr_in client_addr;
         socklen_t          length = sizeof(client_addr);

        int new_server_socket = accept(server_socket, (struct sockaddr*)&client_addr, &length);
         if (new_server_socket < 0)
         {
             printf("Server Accept Failed!\n");
             break;
         }

         pdata.client_addr = client_addr;
         pdata.sock_fd = new_server_socket;
         pthread_create(&pt,NULL,serveForClient,(void *)&pdata);

     }
     close(server_socket);

     return 0;
 }


void *serveForClient(void *arg){
    struct pthread_data *pdata = (struct pthread_data*)arg;
    int new_server_socket = pdata->sock_fd;

    int needRecv=sizeof(Keyboard);
    char *buffer=(char*)malloc(needRecv);
    while(1){
         int pos=0;
         int len;
         memset(keyboard,0,needRecv);
         memset(buffer,0,needRecv);
         while(pos < needRecv) {
             len = recv(new_server_socket, buffer+pos, BUFFER_SIZE, MSG_WAITALL);
             if (len < 0) {
                printf("Server Recieve Data Failed!\n");
                break;
             }
            pos+=len;
         }
         memcpy(keyboard,buffer,needRecv);  
        printf("recv over \n");
        printf("up=%d,down=%d,left=%d,right=%d,mouse_left=%d,mouser_right=%d \n",
             keyboard->up,keyboard->down,
             keyboard->left,keyboard->right,
             keyboard->mouse_left,keyboard->mouse_right);
             
        InputValue::Getinstance()->UpdateValue(
             keyboard->up,keyboard->down,
             keyboard->left,keyboard->right,
             keyboard->mouse_left,keyboard->mouse_right);
        // ClickKey();
        // usleep(15000);

    }
    pthread_exit(0);
}

