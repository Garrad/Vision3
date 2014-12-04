#ifndef IOSTREAM_H_
#define IOSTREAM_H_
#include <iostream>
#endif

#ifndef OPENCV_H_
#define OPENCV_H_
#include "opencv2/opencv.hpp"
#endif

#ifndef MATH_H_
#define MATH_H_
#include <math.h>
#endif

#include "gradients.h"

using namespace std;


// Parameters for motion detection
#define MOVEMENT_THRESHOLD 10


void calcIMatrices(Mat prevFrame, Mat currFrame, Mat &Ix, Mat &Iy, Mat &It) {
	// Calculates required temporal/spatial gradients

	Size imSize = currFrame.size();

	// set initial matrix values
	Ix = currFrame.clone();
	Ix.setTo(Scalar(5));

	Iy = currFrame.clone();
	Iy.setTo(Scalar(5));

	It = currFrame.clone();
	It.setTo(Scalar(5));

	// values to be used
	int Ia;
	int Ib;
	int Ic;
	int Id;
	int result;
	
	// Calculate Ix:
	for (int i=1; i<=imSize.width-1; i++) {
		for (int j=0; j<=imSize.height-1; j++) {
			// Ix value is (Ia+Ib+Ic+Id)/4
			Ia = prevFrame.at<uchar>(j,i) - prevFrame.at<uchar>(j,i-1);
			Ib = prevFrame.at<uchar>(j+1,i) - prevFrame.at<uchar>(j+1,i-1);
			Ic = currFrame.at<uchar>(j,i) - currFrame.at<uchar>(j,i-1);
			Id = currFrame.at<uchar>(j+1,i) - currFrame.at<uchar>(j+1,i-1);

			result = (Ia+Ib+Ic+Id)/4;

			Ix.at<uchar>(j,i) = (result+256)/2;
		}
	}
	
	// Calculate Iy:
	for (int i=0; i<=imSize.width; i++) {
		for (int j=1; j<=imSize.height-1; j++) {
			Ia = prevFrame.at<uchar>(j,i) - prevFrame.at<uchar>(j-1,i);
			Ib = prevFrame.at<uchar>(j,i+1) - prevFrame.at<uchar>(j-1,i+1);
			Ic = currFrame.at<uchar>(j,i) - currFrame.at<uchar>(j-1,i);
			Id = currFrame.at<uchar>(j,i+1) - currFrame.at<uchar>(j-1,i+1);

			result = (Ia+Ib+Ic+Id)/4;

			Iy.at<uchar>(j,i) = (result+256)/2;
		}
	}

	// Calculate It:
	int currVal;
	int prevVal;
	for (int i=1; i<=imSize.width-1; i++) {
		for (int j=1; j<=imSize.height-1; j++) {
			currVal = currFrame.at<uchar>(j,i) + currFrame.at<uchar>(j+1,i-1) + currFrame.at<uchar>(j-1,i-1) + currFrame.at<uchar>(j-1,i+1) + currFrame.at<uchar>(j+1,i+1);
			prevVal = prevFrame.at<uchar>(j,i) + prevFrame.at<uchar>(j+1,i-1) + prevFrame.at<uchar>(j-1,i-1) + prevFrame.at<uchar>(j-1,i+1) + prevFrame.at<uchar>(j+1,i+1);
			result = currVal - prevVal;
			It.at<uchar>(j,i) = (result + 5*256) / (10);
		}
	}
	
}
