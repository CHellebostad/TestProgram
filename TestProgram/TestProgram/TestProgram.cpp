// TestProgram.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <opencv2\opencv.hpp>

using namespace System;
using namespace System::IO::Ports;
using namespace System::Threading;
using namespace std;
using namespace cv;


void static gyroData() {
	SerialPort port;
	port.BaudRate = 115200;
	port.PortName = "COM3";
	port.Open();
	float f;
	char b[4];
	while (true) {
		for (int x = 0; x < 2; x++) {
			for (int i = 0; i < 4; i++) {
				b[i] = port.ReadByte();
			}
			if (x == 0) {
				memcpy(&f, b, sizeof(f));
				cout << "GyroX: " << f;
			}
			else if (x == 1) {
				memcpy(&f, b, sizeof(f));
				cout << " GyroZ: " << f << endl;
			}
		}

	}

}



int main()
{

	Thread^ t0 = gcnew Thread(gcnew ThreadStart(gyroData));
	t0->Start();
	while (true) {

	}

	return 0;
}
