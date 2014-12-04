#include "/usr/local/Cellar/opencv/2.4.9/include/opencv2/objdetect/objdetect.hpp"
#include "/usr/local/Cellar/opencv/2.4.9/include/opencv2/opencv.hpp"
#include "/usr/local/Cellar/opencv/2.4.9/include/opencv2/core/core.hpp"
#include "/usr/local/Cellar/opencv/2.4.9/include/opencv2/highgui/highgui.hpp"
#include "/usr/local/Cellar/opencv/2.4.9/include/opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <stdio.h>


using namespace std;
using namespace cv;
#include <numeric>
#include <random>
#define DRAG 0.9
#define STEP 5
//#define XDIM 400
//#define YDIM 600
#define TARGET_SIZE 50
#define BALL_RADIUS 20
#define DELAY 10

class Target
{

private:
	int xpos;
	int ypos;
	int size;
	int ball_x;
	int ball_y;
	int radius;
	float x_vel;
	float y_vel;
	int x_size;
	int y_size;
	Rect topRect;
	Rect bottomRect;
	Rect sideRect;


public:
	void init(int max_x, int max_y, int area, int radius);
	void draw_target(Mat image);
	void draw_ball(Mat image);
	void move_ball(float x, float y);
	Rect ballRect;
	Rect targetRect;
};