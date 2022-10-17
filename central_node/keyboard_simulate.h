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
// #include <unordered_map>


class PressKey {
public:
    PressKey(KeySym k) {
        alp_ = k;
        press_ = 0;
    }
    KeySym alp_;
    int press_;
};

class KeyboardBase {
public:

    KeyboardBase() {
        pitch_base = 0;
        pitch_th = 10;
        yaw_base = 0;
        yaw_th = 10;
        roll_base = 0;
        roll_th = 10;
        //p1按钮
        keyboardset_.insert(std::make_pair<std::string, PressKey*>("A",  new PressKey(XK_A)));    
        keyboardset_.insert(std::make_pair<std::string, PressKey*>("D", new PressKey(XK_D))); 
        keyboardset_.insert(std::make_pair<std::string, PressKey*>("W", new PressKey(XK_W))); 
        keyboardset_.insert(std::make_pair<std::string, PressKey*>("S", new PressKey(XK_S))); 
        keyboardset_.insert(std::make_pair<std::string, PressKey*>("J", new PressKey(XK_J))); 
        keyboardset_.insert(std::make_pair<std::string, PressKey*>("K", new PressKey(XK_K))); 
        keyboardset_.insert(std::make_pair<std::string, PressKey*>("L", new PressKey(XK_L))); 
        keyboardset_.insert(std::make_pair<std::string, PressKey*>("U", new PressKey(XK_U))); 
        keyboardset_.insert(std::make_pair<std::string, PressKey*>("I", new PressKey(XK_I))); 
        keyboardset_.insert(std::make_pair<std::string, PressKey*>("O", new PressKey(XK_O)));

        //确定，返回，暂停按钮------可以与手势对应
        keyboardset_.insert(std::make_pair<std::string, PressKey*>("Return", new PressKey(XK_Return))); 
        keyboardset_.insert(std::make_pair<std::string, PressKey*>("Escape", new PressKey(XK_Escape))); 
        keyboardset_.insert(std::make_pair<std::string, PressKey*>("space", new PressKey(XK_space))); 

        //p2按钮
        keyboardset_.insert(std::make_pair<std::string, PressKey*>("1", new PressKey(XK_1)));
        keyboardset_.insert(std::make_pair<std::string, PressKey*>("2", new PressKey(XK_2)));
        keyboardset_.insert(std::make_pair<std::string, PressKey*>("3", new PressKey(XK_3)));
        keyboardset_.insert(std::make_pair<std::string, PressKey*>("4", new PressKey(XK_4)));
        keyboardset_.insert(std::make_pair<std::string, PressKey*>("5", new PressKey(XK_5)));
        keyboardset_.insert(std::make_pair<std::string, PressKey*>("6", new PressKey(XK_6)));
        keyboardset_.insert(std::make_pair<std::string, PressKey*>("Up", new PressKey(XK_Up)));
        keyboardset_.insert(std::make_pair<std::string, PressKey*>("Down", new PressKey(XK_Down)));
        keyboardset_.insert(std::make_pair<std::string, PressKey*>("Left", new PressKey(XK_Left)));
        keyboardset_.insert(std::make_pair<std::string, PressKey*>("Right", new PressKey(XK_Right)));
        init_ = false;
    }

    ~KeyboardBase() {
        for(auto it = keyboardset_.begin(); it != keyboardset_.end(); it++) {
            delete it->second;
        }
        std::cout << "all keyboard has been release" <<std::endl;
     }

    int Init() {
        if(init_) {
            std::cout << "error: key board has been init" <<std::endl;
            return -1;
        }
        //获取手势进行感知量初始化
        while(1) {
            if(0){      //预留的手势接口
                std::cout << "\ngesture is success!" << std::endl;
                InitValue(SocketLink::receive_imu_); 
                init_ = true;
                std::cout << "----keyboard init success!----" << std::endl;
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
        while(1) 
        {
            if(imuData == nullptr) {
                std::cout << "----imudata is null----" <<std::endl;
                return;
            }
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
                keyboardset_["J"]->press_ = 1;
                keyboardset_["K"]->press_ = 0;
            }
            else if(del_yaw > 180 && del_yaw < 350) {
                keyboardset_["J"]->press_ = 0;
                keyboardset_["K"]->press_ = 1;       
            }else {
                keyboardset_["J"]->press_ = 0;
                keyboardset_["K"]->press_ = 0; 
            }
            if(del_roll > 10 && del_roll < 180) {
                keyboardset_["D"]->press_ = 1;
                keyboardset_["A"]->press_ = 0;
            }else if(del_roll > 180 && del_roll < 350) {
                keyboardset_["D"]->press_ = 0;
                keyboardset_["A"]->press_ = 1;       
            }else {
                keyboardset_["D"]->press_ = 0;
                keyboardset_["A"]->press_ = 0; 
            }

            ClickKey();
        }
        return;
    }

    void ClickKey() {
 
        Display* p_display = XOpenDisplay( NULL );

        for(auto it = keyboardset_.begin(); it != keyboardset_.end(); it++) {
            KeySym tmp = it->second->alp_;
            KeyCode keycode = NoSymbol;

            keycode = XKeysymToKeycode(p_display, tmp);
            if(it->second->press_ == 1){
                if(it->second->alp_ == XK_W || it->second->alp_ == XK_S ||
                    it->second->alp_ == XK_A || it->second->alp_ == XK_D ||
                    it->second->alp_ == XK_Up || it->second->alp_ == XK_Down ||
                    it->second->alp_ == XK_Left || it->second->alp_ == XK_Right) {      //长按
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
            else {
                XTestFakeKeyEvent( p_display , keycode , False , 0 );   //释放按键
                XFlush( p_display );
                usleep(3000);
            }

        }
        XFlush(p_display);
        XCloseDisplay(p_display);
        usleep(10000);
        
        return;
    }

        

protected:
    std::map<std::string, PressKey*> keyboardset_;
    bool init_;
    float pitch_base;
    float pitch_th;
    float yaw_base;
    float yaw_th;
    float roll_base;
    float roll_th;
};

#endif