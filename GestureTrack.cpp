#include "/usr/include/opencv2/objdetect/objdetect.hpp"
#include "/usr/include/opencv2/opencv.hpp"
#include "/usr/include/opencv2/core/core.hpp"
#include "/usr/include/opencv2/highgui/highgui.hpp"
#include "/usr/include/opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

class GestureTrack
{

	private:
		float noise_sum;
		int noise_count;
		float noise_threshold;
	public:
		void calibrate_noise(float x_sum, float y_sum);

};

void GestureTrack::calibrate_noise(float x_sum, float y_sum)
{
	noise_count++;
	noise_sum += (x_sum*x_sum + y_sum*y_sum);

	noise_threshold = noise_sum / noise_count;
}

int main()
{
	return 1;
}