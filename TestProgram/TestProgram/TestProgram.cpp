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
#include <fstream>

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

void getCameraParams() {
	int numBoards = 10;
	int numCornersHor=9;
	int numCornersVer=6;
	
	int numSquares = numCornersHor*numCornersVer;
	Size board_sz = Size(numCornersHor, numCornersVer);

	vector<vector<Point3f>> object_points;
	vector<vector<Point2f>> image_points;
	vector<Point2f> corners;
	int successes = 0;
	Mat image;
	Mat imageResized;
	Mat imageUndestoredResized;
	Mat gray_image;
	Mat imageUndistorted;
	int indx = 35;
	//cap >> image;
	image = imread("GOPR0035.jpg");
	resize(image, imageResized, Size(640, 480));
	vector<Point3f> obj;
	for (int j = 0; j < numSquares; j++) {
		obj.push_back(Point3f(j / numCornersHor, j%numCornersHor, 0.0f));
	}
	while (successes < numBoards) {
		cvtColor(imageResized, gray_image, CV_BGR2GRAY);
		bool found = findChessboardCorners(imageResized, board_sz, corners, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
		if (found) {
			cornerSubPix(gray_image, corners, Size(11, 11), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 23, 0.1));
			drawChessboardCorners(gray_image, board_sz, corners, found);
		}

		imshow("win1", imageResized);
		imshow("win2", gray_image);
		//cap >> image;
		image = imread("GOPR00" + to_string(indx)+".jpg");
		resize(image, imageResized, Size(640, 480));
		cout << "GOPR00"+to_string(indx)+".jpg" << endl;
		char c = (char)waitKey(33);
		if (c == 27) break;

		//if (c == ' ' && found != 0) {
			if (found != 0) {
			image_points.push_back(corners);
			object_points.push_back(obj);
			printf("Snap stored!");
			successes++;
			if (successes >= numBoards)
				break;
		}
			if (indx == 47) break;
			indx++;
	}
	Mat intrinsic = Mat(3, 3, CV_32FC1);
	Mat distCoeffs;
	vector<Mat> rvecs;
	vector<Mat> tvecs;

	intrinsic.ptr<float>(0)[0] = 0;// 865.6745;
	intrinsic.ptr<float>(1)[1] = 0;// 863.1294;
	calibrateCamera(object_points, image_points, imageResized.size(), intrinsic, distCoeffs, rvecs, tvecs);
	//cout << intrinsic << endl;
	//cout << distCoeffs << endl;
	FileStorage IntrinsicFile("Intrinsic.xml", cv::FileStorage::WRITE);
	IntrinsicFile <<"Intrinsic"<< intrinsic;
	IntrinsicFile.release();
	FileStorage DistCoeffsFile("DistCoeffs.xml", cv::FileStorage::WRITE);
	DistCoeffsFile << "DistCoeffs" << distCoeffs;
	DistCoeffsFile.release();

}

void removeFishEye() {
	Mat image;
	Mat imageResized;
	Mat imageUndistorted;
	Mat intrinsic;
	Mat distCoeffs;
	FileStorage IntrinsicStream("Intrinsic.xml",FileStorage::READ);
	FileStorage DistCoeffsStream("DistCoeffs.xml",FileStorage::READ);
	IntrinsicStream["Intrinsic"] >> intrinsic;
	DistCoeffsStream["DistCoeffs"] >> distCoeffs;
	cout << intrinsic << endl;
	cout << distCoeffs<<endl;

	while (1) {
		cap >> image;
		//image = imread("GOPR0035.jpg");
		resize(image, imageResized, Size(640, 480));
		Mat intrinsicOptimal = getOptimalNewCameraMatrix(intrinsic, distCoeffs, Size(640, 480), 1, Size(640, 480));

		undistort(imageResized, imageUndistorted, intrinsicOptimal, distCoeffs);
		//cout << distCoeffs << endl;
		//cout << intrinsic<<endl;
		imshow("win", imageResized);
		imshow("win2", imageUndistorted);
		waitKey(1);

	}
}



int main()
{
	cap.open(0);

	Thread^ t0 = gcnew Thread(gcnew ThreadStart(removeFishEye));
	t0->Start();
	//getCameraParams();

	//Thread^ t0 = gcnew Thread(gcnew ThreadStart(gyroData));
	//t0->Start();
	//Thread^ t1 = gcnew Thread(gcnew ThreadStart(cropimage));
	//t1->Start();
	return 0;
}
