/////////////////////////////////////////////////////////////////////////////
//
// COMS30121 - video.cpp
// TOPIC: video handling for motion
//
/////////////////////////////////////////////////////////////////////////////

// header inclusion
#include "/usr/local/opencv-2.4/include/opencv2/objdetect/objdetect.hpp"
#include "/usr/local/opencv-2.4/include/opencv2/opencv.hpp"
#include "/usr/local/opencv-2.4/include/opencv2/core/core.hpp"
#include "/usr/local/opencv-2.4/include/opencv2/highgui/highgui.hpp"
#include "/usr/local/opencv-2.4/include/opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;


int main( int argc, const char** argv )
{
	cv::VideoCapture cap;
	if(argc > 1)
	{
		cap.open(string(argv[1]));
	}
	else
	{
		cap.open(CV_CAP_ANY);
	}
	if(!cap.isOpened())
	{
		printf("Error: could not load a camera or video.\n");
	}
	Mat frame;
	namedWindow("video", 1);
	for(;;)
	{
		waitKey(20);
		cap >> frame;
		if(!frame.data)
		{
			printf("Error: no frame data.\n");
			break;
		}
		imshow("video", frame);
	}
}
