#ifndef _imu_data_h
#define _imu_data_h


typedef struct 
{
    float ax;
    float ay;
    float az;

    float wx;
    float wy;
    float wz;

    float roll;
    float pitch;
    float yaw;

    float q_w;
    float q_x;
    float q_y;
    float q_z;
}ImuData;


#endif