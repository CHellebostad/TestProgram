// TestProgram.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <opencv2\opencv.hpp>
#include <iostream>
#include <fstream>

using namespace System;
using namespace System::IO::Ports;
using namespace System::Threading;
using namespace std;
using namespace cv;

ofstream out("out.txt");
float gyroX;
float gyroY;
float gyroZ;
float gyroXcorr;
float gyroYcorr;
float gyroZcorr;
bool first = true;
VideoCapture cap;
VideoWriter writer;

void static gyroData() {
	SerialPort port;
	port.BaudRate = 115200;
	port.PortName = "COM3";
	port.Open();
	float f;
	char b[4];
	while (true) {
		for (int x = 0; x < 3; x++) {
			for (int i = 0; i < 4; i++) {
				b[i] = port.ReadByte();
			}
			if (x == 0) {
				memcpy(&gyroX, b, sizeof(gyroX));
				if (first) {
					gyroXcorr = gyroX;
				}
				out << gyroX-gyroXcorr <<" ";
			}
			else if (x == 1) {
				memcpy(&gyroY, b, sizeof(gyroY));
				if (first) {
					gyroYcorr = gyroY;
				}
				out << gyroY-gyroYcorr<<" ";
			}
			else if (x == 2) {
				memcpy(&gyroZ, b, sizeof(gyroZ));
				if (first) {
					gyroZcorr = gyroZ;
				}
				out << gyroZ - gyroZcorr << endl;
			}
		}
		if (first) {
			first = !first;
		}
	}
	out.close();

}





int main()
{
	Mat frame;
	cap.open(0);
	writer.open("D:\\Kalibrering.avi",
		cap.get(CAP_PROP_FOURCC),
		cap.get(CAP_PROP_FPS),
		Size(cap.get(CAP_PROP_FRAME_WIDTH),cap.get(CAP_PROP_FRAME_HEIGHT)),true);
	
	//Thread^ t0 = gcnew Thread(gcnew ThreadStart(gyroData));
	//t0->Start();
	while (true) {
		cap.read(frame);
		imshow("frame",frame);
		writer.write(frame);
		if (waitKey(30) >= 0) break;
	}
	writer.release();

	return 0;
}
