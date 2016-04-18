// TestProgram.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <opencv2\opencv.hpp>
#include <iostream>
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
VideoCapture cap;
Mat frame;


void static gyroData() {
	SerialPort port;
	port.BaudRate = 115200;
	port.PortName = "COM3";
	port.Open();
	int stack=0;
	char b[4];
	
	while (true) {
				port.DiscardInBuffer();
				for (int i = 0; i < 4; i++) {
					b[i] = port.ReadByte();

				}
					memcpy(&gyroX, b, sizeof(gyroX));
					stackGX =stackGX+gyroX;
					stack++;
					if (stack == 2) {
						deltaGX = stackGX / 2;
						cout << gyroX << endl;
						stack = 0;
						stackGX = 0;
					}
		char c = (char)waitKey(33);
		if (c == 27) break;
	}
}


void cropimage() {
	
	while (true) {
		cap >> frame;
		Mat rotx_mat = getRotationMatrix2D(Point(frame.cols / 2, frame.rows / 2),-deltaGX,1);
		Mat warp;
		warpAffine(frame, warp, rotx_mat,Size(frame.cols,frame.rows));
		imshow("Warped Image", warp);
		char c = (char)waitKey(33);
		if (c == 27) break;
	}
}

void removeFishEye() {
	int numBoards = 0;
	int numCornersHor=9;
	int numCornersVer=6;
	int numSquares = numCornersHor*numCornersVer;
	Size bord_sz = Size(numCornersHor, numCornersVer);
	vector<vector<Point3f>> object_points;
	vector<vector<Point2f>> image_points;
	vector<Point2f> corners;
	Mat image;
	Mat gray_image;
	Mat intrinsic = Mat(3, 3, CV_32FC1);
	Mat distCoeffs = Mat(2, 2, CV_32FC1);
	vector<Mat> rvecs;
	vector<Mat> tvecs;
	intrinsic.ptr<float>(0)[0] = 1520.285040118956;
	intrinsic.ptr<float>(1)[1] = 1501.931438021879;
	//intrinsic.ptr<float>(0)[2] = 1295.589821181276;
	//intrinsic.ptr<float>(1)[2] = 944.1007834224537;
	//intrinsic.ptr<float>(2)[2] = 1;

	frame = imread("img2.jpg");
	Mat rectifisert;
	undistort(frame, rectifisert, intrinsic, distCoeffs);
	imshow("test",rectifisert);
}



int main()
{
	removeFishEye();

	//cap.open(0);
	//Thread^ t0 = gcnew Thread(gcnew ThreadStart(gyroData));
	//t0->Start();
	//Thread^ t1 = gcnew Thread(gcnew ThreadStart(cropimage));
	//t1->Start();
	return 0;
}
