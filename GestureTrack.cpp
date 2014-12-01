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
	Point2f av_motion = Point(x_sum/count, y_sum/count);
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
		sum.x += motion_history.at(i).x;
		sum.y += motion_history.at(i).y;
	}

	mean_point.x = sum.x / motion_history.size();
	mean_point.y = sum.y / motion_history.size();
	/*
	if (mean_point.x != 0 || mean_point.y != 0)
	{
		printf("Average point is: (%f, %f)\n", mean_point.x, mean_point.y);
	}
	*/
	if (mean_point.x > motion_threshold && mean_point.y > motion_threshold)
	{
		printf("Moving down and left\n");
	}
	else if (mean_point.x > motion_threshold && mean_point.y < -motion_threshold)
	{
		printf("Moving up and left\n");
	}
	else if (mean_point.x < -motion_threshold && mean_point.y > motion_threshold)
	{
		printf("Moving down and right\n");
	}
	else if (mean_point.x < -motion_threshold && mean_point.y < -motion_threshold)
	{
		printf("Moving up and right\n");
	}
	else if (mean_point.x < -motion_threshold)
	{
		printf("Moving right\n");
	}
	else if (mean_point.x > motion_threshold)
	{
		printf("Moving left\n");
	}
	else if (mean_point.y < -motion_threshold)
	{
		printf("Moving up\n");
	}
	else if (mean_point.y > motion_threshold)
	{
		printf("Moving down\n");
	}


	return;
}
