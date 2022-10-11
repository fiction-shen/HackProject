#include <iostream>
#include <mutex>
#include <vector>
#include <thread>
#include <unistd.h>



#ifndef STATICVALUE_H
#define STATICVALUE_H


static std::mutex lck;
class InputValue {
public:
    static InputValue* Getinstance() {
        static InputValue instance;
        return &instance;
    }

    void UpdateValue(bool w,bool s,bool a,bool d,bool l,bool r) {
        // lck.lock();
        up = w;
        down = s;
        left = a;
        right = d;
        mouse_left = left;
        mouse_right = right;
        // lck.unlock();
    }

    std::vector<bool> GetValue(){
        lck.lock();
        std::vector<bool> value;
        value.push_back(up);
        value.push_back(down);
        value.push_back(left);
        value.push_back(right);
        
        lck.unlock();
        return value;
    }

    ~InputValue(){}
    bool up=false;
    bool down=false;
    bool left=false;
    bool right=false;
    bool mouse_left=false;
    bool mouse_right=false;
private:
    InputValue(){}
};

// bool InputValue::up = false;
// bool InputValue::down = false;
// bool InputValue::left = false;
// bool InputValue::right = false;
// bool InputValue::mouse_left = false;
// bool InputValue::mouse_right = false;
// std::mutex InputValue::lck;

#endif