#include "GestureTrack.h"

void GestureTrack::calibrate_noise(float x_sum, float y_sum)
{
	noise_count++;
	noise_sum += (x_sum*x_sum + y_sum*y_sum);
	noise_threshold = noise_sum / noise_count;
}

float GestureTrack::get_threshold()
{
	return noise_threshold;
}

void GestureTrack::add_frame(float x_sum, float y_sum, int count)
{
	Point av_motion = Point(x_sum/count, y_sum/count);
	motion_history.push_back(av_motion);
	if (motion_history.size() > window_size)
	{
		motion_history.erase(motion_history.begin());
	}
}

void GestureTrack::detect()
{
	Point2f sum, mean_point;
	int count;
	for (int i = 0;i<motion_history.size();i++)
	{
		sum.x += motion_history.at<Point>.x(i);
		sum.y += motion_history.at<Point>.y(i);
	}

	mean_point.x = sum.x / motion_history.size();
	mean_point.y = sum.y / motion_history.size();
	printf("Average point is: (%f, %f)\n", mean_point.x, mean_point.y);

	return;
}
