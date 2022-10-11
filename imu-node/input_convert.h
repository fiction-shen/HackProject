#ifndef _input_convert_h
#define _input_convert_h

#include "imu_data.h"
#include <memory>
#include <stdlib.h>
#include <math.h>

typedef struct 
{
    bool up;
    bool down;
    bool left;
    bool right;
    bool mouse_left;
    bool mouse_right;
}Keyboard;

class InputConvert{
public:
    InputConvert(float base_yaw=0.0f,float yaw_thresh=5.0f,
        float base_pitch=0.0f,float pitch_thresh=10.0f)
        :base_yaw_(base_yaw),
        yaw_thresh_(yaw_thresh),
        base_pitch_(base_pitch),
        pitch_thresh_(pitch_thresh),
        keyboard_(nullptr),
        imuData_(nullptr){

    }
    void UpdateValue() {
        if (imuData_ == nullptr) {
            std::cout << "no imu msg" << std::endl;
            return;
        }
        base_yaw_ = imuData_->yaw;
        base_pitch_ = imuData_->pitch;
        std::cout << "update success!" <<std::endl;
        return;
    }

    void setKeyboard(std::shared_ptr<Keyboard> keyboard){
        keyboard_ = keyboard;
    }
    void setImuData(std::shared_ptr<ImuData> imuData){
        imuData_ = imuData;
    }
    int getKeyboardInput() {
        if (keyboard_ == nullptr || imuData_ == nullptr) {
            return 0;
        }
        float delta_yaw = imuData_->yaw - base_yaw_;
        if (fabs(delta_yaw) > yaw_thresh_) {
            if (delta_yaw > 0) {
                keyboard_->left = true;
                keyboard_->right = false;
            } else {
                keyboard_->left = false;
                keyboard_->right = true;
            }
        } else {
            keyboard_->left = false;
            keyboard_->right = false;
        }

        float delta_pitch = imuData_->pitch - base_pitch_;
        if (fabs(delta_pitch) > pitch_thresh_) {
            if (delta_pitch > 0) {
                keyboard_->up = false;
                keyboard_->down = true;
            } else {
                keyboard_->up = true;
                keyboard_->down = false;
            }
        } else {
            keyboard_->up = false;
            keyboard_->down = false;
        }
        return 1;
    }
private:
    float base_yaw_;
    float yaw_thresh_;
    float base_pitch_;
    float pitch_thresh_;
    std::shared_ptr<Keyboard> keyboard_;
    std::shared_ptr<ImuData> imuData_;
};

#endif