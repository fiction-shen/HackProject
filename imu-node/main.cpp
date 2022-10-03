
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <memory>
#include "WzSerialPort.h"
#include "im948_CMD.h"
#include "imu_data.h"
#include "send_keyboard.h"
#include "input_convert.h"


using namespace std;


WzSerialPort w;

std::shared_ptr<ImuData> imuData = std::make_shared<ImuData>();


int main(int argumentCount, const char* argumentValues[]) 
{
	char dev_str[12];
	if (argumentCount == 2) {
		sprintf(dev_str,"/dev/ttyUSB%s\0",argumentValues[1]);
	} else {
		sprintf(dev_str,"/dev/ttyUSB0\0");
	}
	
	if (w.open(dev_str, 115200, 0, 8, 1)) {
		Dbp("--- IM948 test start serial port %s\r\n",dev_str);
	} else {
		Dbp("--- IM948 init fail,cannot open serial port %s\r\n",dev_str);
		return;
	}

	Cmd_03();//2.唤醒传感器
	//Cmd_12(8,   6, 5,  0, 1,30, 1, 2, 7, 0x0002);
	Cmd_12(5, 255, 0,  0, 2, 60, 1, 3, 5, 0xFFFF); // 1.设置参数
    Cmd_19();//3.开启数据主动上报

	std::shared_ptr<Keyboard> keyboard = std::make_shared<Keyboard>();

	std::shared_ptr<InputConvert> inputConvert = std::make_shared<InputConvert>();
	inputConvert->setImuData(imuData);
	inputConvert->setKeyboard(keyboard);

	int sock_cli = init_socket();
	Dbp("successful connect link, sock_cli= %d\r\n",sock_cli);


	char buf[4096];

	while (1) {	
		memset(buf, 0,4096);
		int receive_counter = w.receive(buf, 4096);
		if (receive_counter) {
			//Dbp("\nReceive serial port data \r\n");
			for (int i=0; i<receive_counter; i++) {
				if (Cmd_GetPkt(buf[i])) {
					Dbp("Receive one frame of complete data \r\n");			
					if (inputConvert->getKeyboardInput() == 0) {
						Dbp("input convert fail \r\n");	
						break;
					}
					send_msg(sock_cli,keyboard.get(),sizeof(Keyboard));
					break;
				}
				//Dbp("%02x ",buf[i]);
			}
		} 
	}
	return 0;
} 