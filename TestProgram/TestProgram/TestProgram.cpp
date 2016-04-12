// TestProgram.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <opencv2\opencv.hpp>
#include <iostream>
#include <fstream>
#include <opencv2\videoio.hpp>
#include <cmath>
#include <math.h>
#include <ctime>
#include <cstdio>
#include <chrono>

using namespace System;
using namespace System::IO::Ports;
using namespace System::Threading;
using namespace std;
using namespace cv;


float gyroX = 0;
float deltaGX = 0;
float stackGX = 0;
float gyroXcorr = 0;
bool first = true;
VideoCapture cap;
Mat frame;

void static gyroData() {
	SerialPort port;
	port.BaudRate = 115200;
	port.PortName = "COM3";
	port.Open();
	int stack = 0;
	char b[4];

	while (true) {
		for (int x = 0; x < 1; x++) {
			port.DiscardInBuffer();
			for (int i = 0; i < 4; i++) {
				b[i] = port.ReadByte();

			}
			if (x == 0) {
				memcpy(&gyroX, b, sizeof(gyroX));
				stackGX = stackGX + gyroX;
				stack++;
				if (stack == 3) {
					deltaGX = stackGX / 3;
					//cout << gyroX << endl;
					stack = 0;
					stackGX = 0;
				}

			}
			if (first) {
				first = !first;
			}
		
		}
	}
}

void cropimage() {
	cap.open(0);
	while (true) {
		cap >> frame;
		Mat rotx_mat = getRotationMatrix2D(Point(frame.cols / 2, frame.rows / 2), -deltaGX, 1);
		Mat warp;
		warpAffine(frame, warp, rotx_mat, Size(frame.cols, frame.rows));
		imshow("roi_frame", warp);
		char c = (char)waitKey(33);
		if (c == 27) break;
	}
}

int main()
{

	
	Thread^ t0 = gcnew Thread(gcnew ThreadStart(gyroData));
	t0->Start();
	Thread^ t1 = gcnew Thread(gcnew ThreadStart(cropimage));
	t1->Start();

	return 0;
}