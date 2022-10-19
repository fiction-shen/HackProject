#ifndef _IMU_h
#define _IMU_h

#include <memory>

typedef signed char            S8;
typedef unsigned char          U8;
typedef signed short           S16;
typedef unsigned short         U16;
typedef signed long            S32;
typedef unsigned long          U32;
typedef float                  F32;




typedef struct {
    float ax;
    float ay;
    float az;

    float Ax;
    float Ay;
    float Az;

    float wx;
    float wy;
    float wz;

    float mx;
    float my;
    float mz;

    float temperature;
    float air_pressure;
    float height;

    float q_w;
    float q_x;
    float q_y;
    float q_z;

    float roll;
    float pitch;
    float yaw;

    float position_x;
    float position_y;
    float position_z;

    int steps;
    int warking_mode;
    int running_mode;
    int biking_mode;
    int driving_mode;

    float N_ax;
    float N_ay;
    float N_az;

    float ADC;
    int GPIO;
    
}ImuData;


int parse_imu(U8 *buf, U8 DLen, std::shared_ptr<ImuData> imuData);

#endif