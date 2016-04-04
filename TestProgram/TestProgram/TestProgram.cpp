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


ofstream out("out.txt");
float gyroX = 0;
float gyroXgrader = 0;
float gyroXstack = 0;
float gyroY = 0;
float gyroZ = 0;
float deltaGX = 0;
float stackGX = 0;
float oldGX = 0;
float gyroXcorr = 0;
float gyroYcorr = 0;
float gyroZcorr = 0;
float mean = 0;
bool first = true;
VideoCapture cap;
Mat frame;
Mat Kintrisic(3, 3, CV_32FC1);
Mat Kintrisicinvers(3, 3, CV_32FC1);



void warping(float xcoor) {
	

}

void static gyroData() {
	SerialPort port;
	port.BaudRate = 115200;
	port.PortName = "COM3";
	port.Open();
	float f;
	int stack=0;
	char b[4];
	
	while (true) {
			for (int x = 0; x < 1; x++) {
				port.DiscardInBuffer();
				for (int i = 0; i < 4; i++) {
					b[i] = port.ReadByte();

				}
				if (x == 0) {
					memcpy(&gyroX, b, sizeof(gyroX));
					/*stackGX =stackGX+gyroX;
					stack++;*/
					//if (stack == 2) {
					//	deltaGX = stackGX / 2;
						cout << gyroX << endl;
					//	stack = 0;
					//	stackGX = 0;
					//}

					Mat img1, rectified, img_row, result, trans_mat;
					int readout = 0.8;
					float shift = 0;
					img1 = imread("image1.bmp");
					for (int i = 0; i < img1.rows; i++) {
						shift = shift + (gyroX *(3.14/180)*( 1.1 / 480));
						trans_mat = (Mat_<double>(2, 3) << 1, 0, 0 + shift, 0, 1, 0);
						img_row = img1.row(i);
						warpAffine(img_row, rectified, trans_mat, img_row.size());
						result.push_back(rectified);
					}
					imshow("rectified", result);

				/*	if (first) {
						gyroXcorr = gyroX;
					}*/
				}
				/*else if (x == 1) {
					memcpy(&gyroY, b, sizeof(gyroY));
					if (first) {
						gyroYcorr = gyroY;
					}
					cout << gyroY << " ";
				}
				else if (x == 2) {
					memcpy(&gyroZ, b, sizeof(gyroZ));
					if (first) {
						gyroZcorr = gyroZ;
					}
					cout << gyroZ << endl;
				}*/

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
		Mat rotx_mat = getRotationMatrix2D(Point(frame.cols / 2, frame.rows / 2),-deltaGX,1);
		Mat warp;
		warpAffine(frame, warp, rotx_mat,Size(frame.cols,frame.rows));
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

	//cap.open(0);
	Thread^ t0 = gcnew Thread(gcnew ThreadStart(gyroData));
	t0->Start();
	//Thread^ t1 = gcnew Thread(gcnew ThreadStart(cropimage));
	//t1->Start();
	///*Thread^ t2 = gcnew Thread(gcnew ThreadStart(quaternionSlerp));
	//t2->Start();*/
	//Thread^ t3 = gcnew Thread(gcnew ThreadStart(gyroTilGrader));
	//t3->Start();
	//Thread^ t4 = gcnew Thread(gcnew ThreadStart(warping));
	//t4->Start();
	return 0;
}
