// TestProgram.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <opencv2\opencv.hpp>
#include <iostream>
#include <fstream>
#include <opencv2\videoio.hpp>

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
Mat frame;
Mat Kintrisic(3, 3, CV_32FC1);
Mat Kintrisicinvers(3, 3, CV_32FC1);




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
		char c = (char)waitKey(33);
		if (c == 27) break;
	}

	out.close();

}

void cropimage() {
	
	while (true) {
		cap >> frame;
		Mat rotx_mat = getRotationMatrix2D(Point(frame.cols / 2, frame.rows / 2),30,1);
		Mat warp;
		warpAffine(frame, warp, rotx_mat,Size(frame.rows,frame.cols));
		//Rect roi(20, 20, 600,380);
		//Mat roi_frame = warp(roi);
		imshow("roi_frame", warp);
		char c = (char)waitKey(33);
		if (c == 27) break;
	}
}

void captureVideo() {

	int frame_width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	int frame_height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	VideoWriter video("out.avi", CV_FOURCC('M', 'J', 'P', 'G'), 30, Size(frame_width, frame_height), true);
	while (true) {

		cap >> frame;
		video.write(frame);
		imshow("frame", frame);
		char c = (char)waitKey(33);
		if (c == 27) break;
	}
}


int main()
{
	cap.open(0);


	/*Thread^ t0 = gcnew Thread(gcnew ThreadStart(gyroData));
	t0->Start();*/
	Thread^ t1 = gcnew Thread(gcnew ThreadStart(cropimage));
	t1->Start();
	return 0;
}
