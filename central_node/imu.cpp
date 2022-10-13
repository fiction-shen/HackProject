

#include <math.h>
#include <memory>
#include "stdio.h"
#include "imu.h"

// 传输时转换比例--------------
#define scaleAccel       0.00478515625f // 加速度 [-16g~+16g]    9.8*16/32768
#define scaleQuat        0.000030517578125f // 四元数 [-1~+1]         1/32768
#define scaleAngle       0.0054931640625f // 角度   [-180~+180]     180/32768
#define scaleAngleSpeed  0.06103515625f // 角速度 [-2000~+2000]    2000/32768
#define scaleMag         0.15106201171875f // 磁场 [-4950~+4950]   4950/32768
#define scaleTemperature 0.01f // 温度
#define scaleAirPressure 0.0002384185791f // 气压 [-2000~+2000]    2000/8388608
#define scaleHeight      0.0010728836f    // 高度 [-9000~+9000]    9000/8388608
#define pow2(x) ((x)*(x)) // 求平方




int parse_imu(U8 *buf, U8 DLen, std::shared_ptr<ImuData> imuData) {
    U16 ctl;
    U8 L;
    U8 tmpU8;
    U16 tmpU16;
    U32 tmpU32;
    F32 tmpX, tmpY, tmpZ, tmpAbs;

    if (buf[0] != 0x11) {
        return -1;
    }
    
    ctl = ((U16)buf[2] << 8) | buf[1];// 字节[2-1] 为功能订阅标识，指示当前订阅了哪些功能
    printf("\t subscribe tag: 0x%04X\r\n", ctl);
    printf("\t ms: %u\r\n", (U32)(((U32)buf[6]<<24) | ((U32)buf[5]<<16) | ((U32)buf[4]<<8) | ((U32)buf[3]<<0))); // 字节[6-3] 为模块开机后的时间戳(单位ms)

    L =7; // 从第7字节开始根据 订阅标识tag来解析剩下的数据
    if ((ctl & 0x0001) != 0)
    {// 加速度xyz 去掉了重力 使用时需*scaleAccel m/s
        tmpX = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleAccel; L += 2; printf("\taX: %.3f\r\n", tmpX); // x加速度aX
        tmpY = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleAccel; L += 2; printf("\taY: %.3f\r\n", tmpY); // y加速度aY
        tmpZ = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleAccel; L += 2; printf("\taZ: %.3f\r\n", tmpZ); // z加速度aZ
        tmpAbs = sqrt(pow2(tmpX) + pow2(tmpY) + pow2(tmpZ)); printf("\ta_abs: %.3f\r\n", tmpAbs); // 3轴合成的绝对值
        imuData->ax = tmpX;
        imuData->ay = tmpY;
        imuData->az = tmpZ;
    }
    if ((ctl & 0x0002) != 0)
    {// 加速度xyz 包含了重力 使用时需*scaleAccel m/s
        tmpX = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleAccel; L += 2; printf("\tAX: %.3f\r\n", tmpX); // x加速度AX
        tmpY = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleAccel; L += 2; printf("\tAY: %.3f\r\n", tmpY); // y加速度AY
        tmpZ = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleAccel; L += 2; printf("\tAZ: %.3f\r\n", tmpZ); // z加速度AZ
        tmpAbs = sqrt(pow2(tmpX) + pow2(tmpY) + pow2(tmpZ)); printf("\tA_abs: %.3f\r\n", tmpAbs); // 3轴合成的绝对值
        imuData->Ax = tmpX;
        imuData->Ay = tmpY;
        imuData->Az = tmpZ;
    }
    if ((ctl & 0x0004) != 0)
    {// 角速度xyz 使用时需*scaleAngleSpeed °/s
        tmpX = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleAngleSpeed; L += 2; printf("\tGX: %.3f\r\n", tmpX); // x角速度GX
        tmpY = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleAngleSpeed; L += 2; printf("\tGY: %.3f\r\n", tmpY); // y角速度GY
        tmpZ = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleAngleSpeed; L += 2; printf("\tGZ: %.3f\r\n", tmpZ); // z角速度GZ
        tmpAbs = sqrt(pow2(tmpX) + pow2(tmpY) + pow2(tmpZ)); printf("\tG_abs: %.3f\r\n", tmpAbs); // 3轴合成的绝对值       
        imuData->wx = tmpX;
        imuData->wy = tmpY;
        imuData->wz = tmpZ;
    }
    if ((ctl & 0x0008) != 0)
    {// 磁场xyz 使用时需*scaleMag uT
        tmpX = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleMag; L += 2; printf("\tCX: %.3f\r\n", tmpX); // x磁场CX
        tmpY = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleMag; L += 2; printf("\tCY: %.3f\r\n", tmpY); // y磁场CY
        tmpZ = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleMag; L += 2; printf("\tCZ: %.3f\r\n", tmpZ); // z磁场CZ
        tmpAbs = sqrt(pow2(tmpX) + pow2(tmpY) + pow2(tmpZ)); printf("\tC_abs: %.3f\r\n", tmpAbs); // 3轴合成的绝对值
        imuData->mx = tmpX;
        imuData->my = tmpY;
        imuData->mz = tmpZ;
    }
    if ((ctl & 0x0010) != 0)
    {// 温度 气压 高度
        tmpX = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleTemperature; L += 2; printf("\ttemperature: %.2f\r\n", tmpX); // 温度

        tmpU32 = (U32)(((U32)buf[L+2] << 16) | ((U32)buf[L+1] << 8) | (U32)buf[L]);
        tmpU32 = ((tmpU32 & 0x800000) == 0x800000)? (tmpU32 | 0xff000000) : tmpU32;// 若24位数的最高位为1则该数值为负数，需转为32位负数，直接补上ff即可
        tmpY = (S32)tmpU32 * scaleAirPressure; L += 3; printf("\tairPressure: %.3f\r\n", tmpY); // 气压

        tmpU32 = (U32)(((U32)buf[L+2] << 16) | ((U32)buf[L+1] << 8) | (U32)buf[L]);
        tmpU32 = ((tmpU32 & 0x800000) == 0x800000)? (tmpU32 | 0xff000000) : tmpU32;// 若24位数的最高位为1则该数值为负数，需转为32位负数，直接补上ff即可
        tmpZ = (S32)tmpU32 * scaleHeight; L += 3; printf("\theight: %.3f\r\n", tmpZ); // 高度

        imuData->temperature = tmpX;
        imuData->air_pressure = tmpY;
        imuData->height = tmpZ;
    }
    if ((ctl & 0x0020) != 0)
    {// 四元素 wxyz 使用时需*scaleQuat
        tmpAbs = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleQuat; L += 2; printf("\tz: %.3f\r\n", tmpAbs); // w
        tmpX =   (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleQuat; L += 2; printf("\tw: %.3f\r\n", tmpX); // x
        tmpY =   (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleQuat; L += 2; printf("\tx: %.3f\r\n", tmpY); // y
        tmpZ =   (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleQuat; L += 2; printf("\ty: %.3f\r\n", tmpZ); // z

        imuData->q_w = tmpAbs;
        imuData->q_x = tmpX;
        imuData->q_y = tmpY;
        imuData->q_z = tmpZ;
    }
    if ((ctl & 0x0040) != 0)
    {// 欧拉角xyz 使用时需*scaleAngle
        tmpX = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleAngle; L += 2; printf("\tangleX: %.3f\r\n", tmpX); // x角速
        tmpY = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleAngle; L += 2; printf("\tangleY: %.3f\r\n", tmpY); // y角速
        tmpZ = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleAngle; L += 2; printf("\tangleZ: %.3f\r\n", tmpZ); // z角速

        imuData->roll = tmpX;
        imuData->pitch = tmpY;
        imuData->yaw = tmpZ;
    }
    if ((ctl & 0x0080) != 0)
    {// xyz 空间位移 单位mm 转为 m
        tmpX = (S16)(((S16)buf[L+1]<<8) | buf[L]) / 1000.0f; L += 2; printf("\toffsetX: %.3f\r\n", tmpX); // x坐标
        tmpY = (S16)(((S16)buf[L+1]<<8) | buf[L]) / 1000.0f; L += 2; printf("\toffsetY: %.3f\r\n", tmpY); // y坐标
        tmpZ = (S16)(((S16)buf[L+1]<<8) | buf[L]) / 1000.0f; L += 2; printf("\toffsetZ: %.3f\r\n", tmpZ); // z坐标

        imuData->position_x = tmpX;
        imuData->position_y = tmpY;
        imuData->position_z = tmpZ;
    }
    if ((ctl & 0x0100) != 0)
    {// 活动检测数据
        tmpU32 = (U32)(((U32)buf[L+3]<<24) | ((U32)buf[L+2]<<16) | ((U32)buf[L+1]<<8) | ((U32)buf[L]<<0)); L += 4; printf("\tsteps: %u\r\n", tmpU32); // 计步数   
        tmpU8 = buf[L]; L += 1;
        printf("\t walking: %s\r\n", (tmpU8 & 0x01)?  "yes" : "no"); // 是否在走路
        printf("\t running: %s\r\n", (tmpU8 & 0x02)?  "yes" : "no"); // 是否在跑步
        printf("\t biking: %s\r\n",  (tmpU8 & 0x04)?  "yes" : "no"); // 是否在骑车
        printf("\t driving: %s\r\n", (tmpU8 & 0x08)?  "yes" : "no"); // 是否在开车

        imuData->steps = tmpU32;
        imuData->warking_mode = (tmpU8 & 0x01);
        imuData->running_mode = (tmpU8 & 0x02);
        imuData->biking_mode = (tmpU8 & 0x04);
        imuData->driving_mode = (tmpU8 & 0x08);
    }
    if ((ctl & 0x0200) != 0)
    {// 加速度xyz 去掉了重力 使用时需*scaleAccel m/s
        tmpX = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleAccel; L += 2; printf("\tasX: %.3f\r\n", tmpX); // x加速度asX
        tmpY = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleAccel; L += 2; printf("\tasY: %.3f\r\n", tmpY); // y加速度asY
        tmpZ = (S16)(((S16)buf[L+1]<<8) | buf[L]) * scaleAccel; L += 2; printf("\tasZ: %.3f\r\n", tmpZ); // z加速度asZ
        tmpAbs = sqrt(pow2(tmpX) + pow2(tmpY) + pow2(tmpZ)); printf("\tas_abs: %.3f\r\n", tmpAbs); // 3轴合成的绝对值
        imuData->N_ax = tmpX;
        imuData->N_ay = tmpY;
        imuData->N_az = tmpZ;
    }
    if ((ctl & 0x0400) != 0)
    {// ADC的值
        tmpU16 = (U16)(((U16)buf[L+1]<<8) | ((U16)buf[L]<<0)); L += 2; printf("\tadc: %u\r\n", tmpU16); // 12位精度ADC的数值(0-4095),对应的电压量程由0x24指令设定
        imuData->ADC = tmpU16;
    }
    if ((ctl & 0x0800) != 0)
    {// GPIO1的值
        tmpU8 = buf[L]; L += 1;
        printf("\t GPIO1  M:%X, N:%X\r\n", (tmpU8>>4)&0x0f, (tmpU8)&0x0f);
        imuData->GPIO = tmpU8;
    }

    return 1;
}