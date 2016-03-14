// SIFT-RANSAC.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <opencv2\opencv.hpp>
#include <opencv2\features2d.hpp>
#include <vl\generic.h>

using namespace System;
using namespace std;
using namespace cv;

Mat frame;
Mat grayFrame;
VideoCapture cap;
Feature2D feature;
FeatureDetector detector;
int main()
{
	if (!cap.open("out.avi")) {
		cout << "Failed to load video" << endl;
	}
	while (true) {
		cap >> frame;
		cvtColor(frame,grayFrame, COLOR_BGR2GRAY);
		imshow("frame", grayFrame);
	}
	return 0;
}

