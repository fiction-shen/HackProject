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
#include "gesture.h"
#include <semaphore.h>
#include <chrono>
#include <unordered_map>
#include <mutex>
#include <thread>


using namespace std::chrono;
steady_clock::time_point time1;

class PressKey {
public:
    PressKey(KeySym k) {
        alp_ = k;
        press_ = 0;
        press_last_ = 0;
    }
    KeySym alp_;
    int press_;
    int press_last_;
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
        imuinit_ = false;
        gesture_ = 0;
        last_pressed_key = nullptr;
        p_display = XOpenDisplay( NULL );
    }

    ~KeyboardBase() {
        for(auto it = keyboardset_.begin(); it != keyboardset_.end(); it++) {
            delete it->second;
        }
        std::cout << "all keyboard has been release" <<std::endl;
     }

//     int Init() {
//         if(imuinit_) {
//             std::cout << "warring: key board has been init" <<std::endl;
//             return -1;
//         }
//         //获取手势进行感知量初始化
// //        while(1) 
//         {
// //            if(SocketLink::receive_gst_->gstcode == 11){      //预留的手势接口
//             //if(1){
//                 // std::cout << "\ngesture is success!" << std::endl;
//                 InitValue(SocketLink::receive_imu_); 
//                 imuinit_ = true;
//                 gesture_ = 11;
//                 std::cout << "----keyboard init success!----" << std::endl;
//                 usleep(500000);
//                 return 0;
//             // } 
//             // else {
//             //     std::cout << "\r----please make a ok gesture----";
//             //     std::fflush(stdout);
//             // }
//             usleep(500000);
//         } 
//         return -1;
//     }

    void InitValue(std::shared_ptr<ImuData> imuData) {
        if(imuinit_) {
            std::cout << "---keyboard has been init already---" << std::endl;
            return;
        }
        
        roll_base = imuData->roll;
        yaw_base = imuData->yaw;
        pitch_base = imuData->pitch;
        imuinit_ = true;
        std::cout << "----keyboard init success!----" << std::endl;
    }

    // void Proc() {
    //     while(1) {
    //         ProcMsg(SocketLink::receive_imu_);
    //         // ProGst();
    //     }
    //     return;
    // }
    void resetKeyboard() {
        for(auto it = keyboardset_.begin(); it != keyboardset_.end(); it++) {
            it->second->press_last_ = it->second->press_;
            it->second->press_ = 0;
        }
    }
    void ProcMsg(std::shared_ptr<ImuData> imuData) {
        while(1) 
        {
            
            if(imuData == nullptr) {
                std::cout << "----imudata is null----" <<std::endl;
            }
            if(!imuinit_) {
                std::cout << "\r---keyboard has no init----please give me a ok gesture---";
                std::fflush(stdout);
            }
            else {
                float roll = imuData->roll;
                float pitch = imuData->pitch;
                float yaw = imuData->yaw;
                int click = (imuData->GPIO)&0x0f;
                // std::cout << "click=" << click << std::endl; 
                float del_yaw = yaw - yaw_base + 360;
                float del_pitch = pitch - pitch_base + 360;
                del_yaw = fmod(del_yaw, 360);
                del_pitch = fmod(del_pitch, 360);
                
                resetKeyboard();
                if(!click) {
                    keyboardset_["K"]->press_ = 1;
                } else {
                    keyboardset_["K"]->press_ = 0;
                }

                if(del_yaw > 10 && del_yaw < 180) {
                    keyboardset_["A"]->press_ = 1;
                    keyboardset_["D"]->press_ = 0;
                }
                else if(del_yaw > 180 && del_yaw < 350) {
                    keyboardset_["A"]->press_ = 0;
                    keyboardset_["D"]->press_ = 1;       
                }
                else {
                    keyboardset_["A"]->press_ = 0;
                    keyboardset_["D"]->press_ = 0; 
                }
                if(del_pitch > 10 && del_pitch < 180) {
                    keyboardset_["J"]->press_ = 0;
                    // keyboardset_["A"]->press_ = 0;
                }else if(del_pitch > 180 && del_pitch < 350) {
                    keyboardset_["J"]->press_ = 1;
                    // keyboardset_["A"]->press_ = 1;       
                }else {
                    keyboardset_["J"]->press_ = 0;
                    // keyboardset_["A"]->press_ = 0; 
                }
                auto startTime = std::chrono::high_resolution_clock::now(); 
                // do something ...
                ClickKey();
                auto endTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - startTime).count();
                auto timeuse = endTime * 0.001;
                //printf("Function Used %f ms\n", timeuse);
                
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(1));  //10ms一次
            
        }
        return;
    }

    void ProGst(std::shared_ptr<GstData> gstdata) {
        while(1) 
        {
            // dlok.lock();

            int num = gstdata->gstcode;
           // std::cout << "num = " << num << std::endl;
            if (num != gesture_){
               // Display* p_display = XOpenDisplay( NULL );
                KeyCode keycode = NoSymbol;
                switch(num) {
                    case 11 : {
                        std::cout << "gest 11\n";
                        InitValue(SocketLink::receive_imu_);
                        break;
                    }   
                    case 4 : {
                        std::cout << "gest 4\n";
                        KeySym tmp = XK_F1;
                        keycode = XKeysymToKeycode(p_display, tmp);
                        break;//嘘，开关声音
                    }     
                    case 12 : {
                        std::cout << "gest 12\n";
                        KeySym tmp = XK_A;
                        keycode = XKeysymToKeycode(p_display, tmp);
                        break;//大拇指向左，选择向左
                    }   
                    case 13 : {
                        std::cout << "gest 13\n";
                        KeySym tmp = XK_D;
                        keycode = XKeysymToKeycode(p_display, tmp);
                        break;//大拇指向右，选择向右
                    }
                    case 5 : {
                        std::cout << "gest 5\n";
                        KeySym tmp = XK_space;;
                        keycode = XKeysymToKeycode(p_display, tmp);
                        break;//手掌，暂停游戏
                    }
                    case 3 : {
                        std::cout << "gest 3\n";
                        KeySym tmp = XK_Return;
                        keycode = XKeysymToKeycode(p_display, tmp);
                        break;//比耶，确定
                    }
                    case 14 : {
                        std::cout << "gest 14\n";
                        KeySym tmp = XK_Escape;
                        keycode = XKeysymToKeycode(p_display, tmp);
                        break;//666，离开游戏
                    }
                    default : {
                        break;
                    }
                }

                gesture_ = num;
                // XTestFakeKeyEvent(p_display , keycode , True  , 0 );
                // XFlush( p_display );
                // std::this_thread::sleep_for(std::chrono::milliseconds(1));  //1ms一次
                // XTestFakeKeyEvent(p_display , keycode , False , 0 );
                // XFlush( p_display );
                // std::this_thread::sleep_for(std::chrono::milliseconds(1));  //1ms一次
                // XFlush(p_display);
                // XCloseDisplay(p_display); 
            }
         
            // dlok.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));  //10毫秒执行一次。  
        }
        return;
    }


    void ClickKey() {
 
        // Display* p_display = XOpenDisplay( NULL );
        // dlok.lock();
        //释放长按的按键
        if(last_pressed_key != nullptr && last_pressed_key->press_ == 0) {
            KeyCode keycode_tmp = NoSymbol;
            keycode_tmp = XKeysymToKeycode(p_display, last_pressed_key->alp_);
            XTestFakeKeyEvent(p_display , keycode_tmp , False  , 0 );
            XFlush( p_display );
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        
        for(auto it = keyboardset_.begin(); it != keyboardset_.end(); it++) {
            KeySym tmp = it->second->alp_;
            KeyCode keycode = NoSymbol;
            keycode = XKeysymToKeycode(p_display, tmp);         

            if(it->second->press_ == 1){    
                //一个player应该只有一个长按的按键         
                if(it->second->alp_ == XK_W || it->second->alp_ == XK_S ||
                    it->second->alp_ == XK_A || it->second->alp_ == XK_D ||
                    it->second->alp_ == XK_Up || it->second->alp_ == XK_Down ||
                    it->second->alp_ == XK_Left || it->second->alp_ == XK_Right) {      //长按
                   
                    // steady_clock::time_point time2 = steady_clock::now();
                    // duration<double> time_span = duration_cast<duration<double>>(time2-time1);
                    // printf("It took me %lf ms\n",time_span*1000);           
                    last_pressed_key = it->second;
                    XTestFakeKeyEvent(p_display , keycode , True  , 0 );
                    XFlush( p_display );
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));                       
                                     
                                      
                } else {
                    XTestFakeKeyEvent( p_display , keycode , True  , 0 ); // 点按
                    XFlush( p_display );
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    XTestFakeKeyEvent( p_display , keycode , False , 0 );
                    XFlush( p_display );
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));

                    // time1 = steady_clock::now();
                }
            } 
            else {
                // if(it->second->alp_ == XK_W || it->second->alp_ == XK_S ||
                //     it->second->alp_ == XK_A || it->second->alp_ == XK_D ||
                //     it->second->alp_ == XK_Up || it->second->alp_ == XK_Down ||
                //     it->second->alp_ == XK_Left || it->second->alp_ == XK_Right) {
                       
                //             XTestFakeKeyEvent( p_display , keycode , False , 0 );   //释放按键
                //             XFlush( p_display );
                //             usleep(1000);
                                        
                //     }      
            }

        }
        
        

        //XFlush(p_display);
        //XCloseDisplay(p_display);
        // dlok.unlock();
        return;
    }

        

protected:
    std::map<std::string, PressKey*> keyboardset_;
    bool imuinit_;
    float pitch_base;
    float pitch_th;
    float yaw_base;
    float yaw_th;
    float roll_base;
    float roll_th;
    int gesture_;
    std::mutex dlok;

    Display* p_display ;
    PressKey* last_pressed_key;

};

#endif



                
                
                