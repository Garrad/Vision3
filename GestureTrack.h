/*
#include "/usr/local/Cellar/opencv/2.4.9/include/opencv2/objdetect/objdetect.hpp"
#include "/usr/local/Cellar/opencv/2.4.9/include/opencv2/opencv.hpp"
#include "/usr/local/Cellar/opencv/2.4.9/include/opencv2/core/core.hpp"
#include "/usr/local/Cellar/opencv/2.4.9/include/opencv2/highgui/highgui.hpp"
#include "/usr/local/Cellar/opencv/2.4.9/include/opencv2/imgproc/imgproc.hpp"*/
#include "opencv2/opencv.hpp"

#include <iostream>
#include <stdio.h>
#include <numeric>

using namespace std;
using namespace cv;

class GestureTrack
{

	private:
		float noise_sum;
		int noise_count;
		float noise_threshold;

	public:
		float motion_threshold;
		int window_size;
		vector<Point> motion_history;
		void calibrate_noise(float x_sum, float y_sum);
		float get_threshold();
		void add_frame(float x_sum, float y_sum, int count);
		Point2f detect();

};