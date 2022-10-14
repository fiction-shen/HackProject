#ifndef __KEYBOARD_SIMULATE_H_
#define __KEYBOARD_SIMULATE_H_

#include <iostream>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include <unistd.h>
#include <math.h>
#include <map>
#include "imu.h"
#include "socket_link.h"

class KeyboardBase {
public:

    KeyboardBase() {
        pitch_base = 0;
        pitch_th = 10;
        yaw_base = 0;
        yaw_th = 10;
        roll_base = 0;
        roll_th = 10;
        keyboardset_.insert(std::make_pair<KeySym, int>(XK_A, 0));    
        keyboardset_.insert(std::make_pair<KeySym, int>(XK_D, 0));
        keyboardset_.insert(std::make_pair<KeySym, int>(XK_W, 0));
        keyboardset_.insert(std::make_pair<KeySym, int>(XK_S, 0));
        keyboardset_.insert(std::make_pair<KeySym, int>(XK_J, 0));
        keyboardset_.insert(std::make_pair<KeySym, int>(XK_K, 0));
        keyboardset_.insert(std::make_pair<KeySym, int>(XK_L, 0));
        keyboardset_.insert(std::make_pair<KeySym, int>(XK_U, 0));
        keyboardset_.insert(std::make_pair<KeySym, int>(XK_I, 0));
        keyboardset_.insert(std::make_pair<KeySym, int>(XK_O, 0));
        keyboardset_.insert(std::make_pair<KeySym, int>(XK_Return, 0));
        keyboardset_.insert(std::make_pair<KeySym, int>(XK_Escape, 0));
        keyboardset_.insert(std::make_pair<KeySym, int>(XK_space, 0));
        keyboardset_.insert(std::make_pair<KeySym, int>(XK_1, 0));
        keyboardset_.insert(std::make_pair<KeySym, int>(XK_2, 0));
        keyboardset_.insert(std::make_pair<KeySym, int>(XK_3, 0));
        keyboardset_.insert(std::make_pair<KeySym, int>(XK_4, 0));
        keyboardset_.insert(std::make_pair<KeySym, int>(XK_5, 0));
        keyboardset_.insert(std::make_pair<KeySym, int>(XK_6, 0));
        keyboardset_.insert(std::make_pair<KeySym, int>(XK_Up, 0));
        keyboardset_.insert(std::make_pair<KeySym, int>(XK_Down, 0));
        keyboardset_.insert(std::make_pair<KeySym, int>(XK_Left, 0));
        keyboardset_.insert(std::make_pair<KeySym, int>(XK_Right, 0));
        init_ = false;
    }

    ~KeyboardBase() { }

    int Init() {
        if(init_) {
            std::cout << "has been init" <<std::endl;
            return -1;
        }
        //获取手势进行感知量初始化
        while(1) {
            if(1){
                std::cout << "\ngesture is success!" << std::endl;
                InitValue(SocketLink::receive_imu_); 
                init_ = true;
                return 0;
            } else {
                std::cout << "\r----please make a start gesture----";
                std::fflush(stdout);
            }
            usleep(500000);
        } 
        return -1;
    }

    void InitValue(std::shared_ptr<ImuData> imuData) {
        roll_base = imuData->roll;
        yaw_base = imuData->yaw;
        pitch_base = imuData->pitch;
    }

    void ProcMsg(std::shared_ptr<ImuData> imuData) {
        if(!init_) {
            std::cout << "----keyboard has no init----" << std::endl;
            Init();
            // return;
        }
        float roll = imuData->roll;
        float pitch = imuData->pitch;
        float yaw = imuData->yaw;
        float del_yaw = yaw - yaw_base + 360;
        float del_roll = roll - roll_base + 360;
        del_yaw = fmod(del_yaw, 360);
        del_roll = fmod(del_roll, 360);

        if(del_yaw > 10 && del_yaw < 180) {
            keyboardset_[XK_J] = 1;
            keyboardset_[XK_K] = 0;
        }else if(del_yaw > 180 && del_yaw < 350) {
            keyboardset_[XK_J] = 0;
            keyboardset_[XK_K] = 1;       
        }else {
            keyboardset_[XK_J] = 0;
            keyboardset_[XK_K] = 0; 
        }
        if(del_roll > 10 && del_roll < 180) {
            keyboardset_[XK_D] = 1;
            keyboardset_[XK_A] = 0;
        }else if(del_roll > 180 && del_roll < 350) {
            keyboardset_[XK_D] = 0;
            keyboardset_[XK_A] = 1;       
        }else {
            keyboardset_[XK_D] = 0;
            keyboardset_[XK_A] = 0; 
        }
        ClickKey();

    }

    void ClickKey() {
        Display* p_display = XOpenDisplay( NULL );
        KeyCode keycode = NoSymbol;
        for(auto it = keyboardset_.cbegin(); it != keyboardset_.cend(); it++) {
            if(it->second == 1){
                keycode = XKeysymToKeycode(p_display, it->first);
                if(it->first == XK_W || it->first == XK_S ||
                    it->first == XK_A || it->first == XK_D ||
                    it->first == XK_Up || it->first == XK_Down ||
                    it->first == XK_Left || it->first == XK_Right) {      //长按
                    XTestFakeKeyEvent(p_display , keycode , True  , 0 );
                    XFlush( p_display );
                    usleep(3000);
                } else {
                    XTestFakeKeyEvent( p_display , keycode , True  , 0 ); // 点按
                    XFlush( p_display );
                    usleep(3000);
                    XTestFakeKeyEvent( p_display , keycode , False , 0 );
                    XFlush( p_display );
                    usleep(3000);
                }
            }

        }
        XCloseDisplay( p_display );
        usleep(10000);
        return;
    }

        

protected:
    std::map<KeySym, int> keyboardset_;
    bool init_;
    float pitch_base;
    float pitch_th;
    float yaw_base;
    float yaw_th;
    float roll_base;
    float roll_th;
};

#endif