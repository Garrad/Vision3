#include "/usr/include/opencv2/objdetect/objdetect.hpp"
#include "/usr/include/opencv2/opencv.hpp"
#include "/usr/include/opencv2/core/core.hpp"
#include "/usr/include/opencv2/highgui/highgui.hpp"
#include "/usr/include/opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <stdio.h>


using namespace std;
using namespace cv;
#include <numeric>
#include <random>

class Target
{

private:
	int xpos;
	int ypos;
	int size;
	int ball_x;
	int ball_y;

public:
	void make_target(int max_x, int max_y, int area);
	void draw_target(Mat image);
	void draw_ball(Mat image, int radius);
};

void Target::make_target(int max_x, int max_y, int area)
{

	size = area;
	xpos = rand() % (max_x - size);
	ypos = rand() % (max_y - size);

	ball_x = max_x / 2;
	ball_y = max_y / 2;

	printf("Goal coord is (%d, %d)\n", xpos, ypos);
}

void Target::draw_target(Mat image)
{
	Scalar white = CV_RGB(255,255,255);

	printf("Line 1 has co-ords: (%d, %d), (%d, %d)", xpos, ypos, xpos-size, ypos);

	line(image, Point(xpos, ypos), Point(xpos+size, ypos), white, 1);
	line(image, Point(xpos+size, ypos), Point(xpos+size, ypos+size), white, 1);
	line(image, Point(xpos, ypos+size), Point(xpos+size, ypos+size), white, 1);

}

void Target::draw_ball(Mat image, int radius)
{
	Scalar white = CV_RGB(255,255,255);

	circle(image, Point(ball_x, ball_y), radius, white);
}

int main()
{
	Target T;
	T.make_target(800, 600, 50);

	Mat img;
	img = Mat::zeros(800, 600, CV_8UC1);

	int key;


	for (;;)
	{
		T.draw_target(img);
		T.draw_ball(img, 20);

		namedWindow("Ball", 1);

		imshow("Ball", img);
		key = waitKey(0);

		if (key == 'q')
		{
			break;
		}
		else if (key == 'UP')
	}

}
