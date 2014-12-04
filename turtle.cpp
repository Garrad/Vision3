#include "turtle.h"

void Target::init(int max_y, int max_x, int area, int rad)
{

	//Store image dims
	x_size = max_x;
	y_size = max_y;

    //Create target
	size = area;
	xpos = rand() % (max_x - size);
	ypos = rand() % (max_y - size);
	//Create colliders for target walls
	topRect = cvRect(xpos, ypos, size, 1);
	bottomRect = cvRect(xpos, ypos+size, size, 1);
	sideRect = cvRect(xpos+size, ypos, 1, size);
	targetRect = cvRect(xpos, ypos, size, size);

    //Set initial x, y position
	ball_x = max_x / 4;
	ball_y = max_y / 4;

	//Set initial velocity
	x_vel = 0;
	y_vel = 0;

	//Store radius
	radius = rad;

	printf("Goal coord is (%d, %d)\n", xpos, ypos);
}

void Target::draw_target(Mat image)
{
	Scalar white = CV_RGB(255,255,255);
    //Draw target
	line(image, Point(xpos, ypos), Point(xpos+size, ypos), white, 1);
	line(image, Point(xpos+size, ypos), Point(xpos+size, ypos+size), white, 1);
	line(image, Point(xpos, ypos+size), Point(xpos+size, ypos+size), white, 1);
	//Draw colliding rectangles when debugging
	//rectangle(image, topRect, white);
	//rectangle(image, bottomRect, white);
	//rectangle(image, sideRect, white);

}

void Target::draw_ball(Mat image)
{
	Scalar white = CV_RGB(255,255,255);
	//Draw ball
	circle(image, Point(ball_x, ball_y), radius, white);
	//printf("Ball pos is (%d, %d)", ball_x, ball_y);
	//Draw bounding rect when debugging
	//rectangle(image, ballRect, white);
}

void Target::move_ball(float x, float y)
{
	//Save old  position
	int old_x = ball_x;
	int old_y = ball_y;

	//Update velocity to include push command and frictional slowing
	x_vel = ((x_vel * DRAG) + x);
	y_vel = ((y_vel * DRAG) + y);

	//Ensure velocity mag is less than radius; prevents teleporting through the walls
	if (x_vel > radius)
	{
		x_vel = radius;
	}
	else if (x_vel < -radius)
	{
		x_vel = -radius;
	}

	if (y_vel > radius)
	{
		y_vel = radius;
	}
	else if (y_vel < -radius)
	{
		y_vel = -radius;
	}

	//Update position ensuring we stay within image bounds
	ball_x = (ball_x + (int) x_vel) % x_size;
	ball_y = (ball_y + (int) y_vel) % y_size;

	if (ball_x < 0)
	{
		ball_x = x_size;
	}

	if (ball_y < 0)
	{
		ball_y = y_size;
	}

    //Check for collisions with walls
	ballRect = cvRect(ball_x-radius, ball_y-radius, 2*radius, 2*radius);

	Rect topInter = ballRect & topRect;
	Rect bottomInter = ballRect & bottomRect;
	Rect sideInter = ballRect & sideRect;

	if (topInter.height == 0 && topInter.width == 0  && bottomInter.height == 0 && bottomInter.width == 0 && sideInter.height == 0 && sideInter.width == 0)
	{
		return;
	}
	else
	{
		//Prevent movement if we hit a wall
		ball_x = old_x;
		ball_y = old_y;

		//Bounce off wall - not well implemented
		x_vel = -x_vel;
		y_vel = -y_vel;
	}
}
/*
int main()
{
	//Create target
	Target T;
	T.init(XDIM, YDIM, TARGET_SIZE, BALL_RADIUS);

	Mat img;
	img = Mat::zeros(XDIM, YDIM, CV_8UC1);

	int key;

	for (;;)
	{

		img = Mat::zeros(XDIM, YDIM, CV_8UC1);
		T.draw_target(img);
		T.draw_ball(img);

		namedWindow("Ball", 1);

		imshow("Ball", img);

		key = waitKey(DELAY);

		if (key == 'q')
		{
			break;
		}
		else if (key == 'w')
		{
			T.move_ball(0, -STEP);
		}
		else if (key == 'a')
		{
			T.move_ball(-STEP, 0);
		}
		else if (key == 's')
		{
			T.move_ball(STEP, 0);
		}
		else if (key == 'z')
		{
			T.move_ball(0, STEP);
		}
		else
		{
			T.move_ball(0, 0);
		}
	}

}
*/