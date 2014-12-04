#include "/usr/include/opencv2/objdetect/objdetect.hpp"
#include "/usr/include/opencv2/opencv.hpp"
#include "/usr/include/opencv2/core/core.hpp"
#include "/usr/include/opencv2/highgui/highgui.hpp"
#include "/usr/include/opencv2/imgproc/imgproc.hpp"

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
		const static float motion_threshold=1.0;
		const static int window_size = 20;
	public:
		vector<Point> motion_history;
		void calibrate_noise(float x_sum, float y_sum);
		float get_threshold();
		void add_frame(float x_sum, float y_sum, int count);
		void detect();

};