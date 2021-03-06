
/////////////////////////////////////////////////////////////////////////////
//
// COMS30121 - video.cpp
// TOPIC: video handling for motion
//
/////////////////////////////////////////////////////////////////////////////

// header inclusion
/*
#include "/usr/local/Cellar/opencv/2.4.9/include/opencv2/objdetect/objdetect.hpp"
#include "/usr/local/Cellar/opencv/2.4.9/include/opencv2/opencv.hpp"
#include "/usr/local/Cellar/opencv/2.4.9/include/opencv2/core/core.hpp"
#include "/usr/local/Cellar/opencv/2.4.9/include/opencv2/highgui/highgui.hpp"
#include "/usr/local/Cellar/opencv/2.4.9/include/opencv2/imgproc/imgproc.hpp"*/
#include "opencv2/opencv.hpp"

#include <iostream>
#include <stdio.h>
#include "GestureTrack.h"

#include "gradients.h"
#include "turtle.h"
#define MAX_FRAME 1000

using namespace std;
using namespace cv;


//Region is patch averaged over to get motion estimate
//ASSUMPTION: Patch has constant motion
//Resolution is spatial resolution of image vectors

// COMMAND LINE INPUTS
// Region - region size
// Resolution - resolution
// ShowDerivatives - 1 shows Ix, Iy, It images, 0 hides
// [video filename] - [optional path to video file]

//Function headers
float Ix(Mat frame_t, Mat frame_t2, int x, int y);
float Iy(Mat frame_t, Mat frame_t2, int x, int y);
float It(Mat frame_t, Mat frame_t2, int x, int y);
Mat LkTracker(Mat frame_t, Mat frame_t2, int x, int y, int size);
static void arrowedLine(Mat img, Point pt1, Point pt2, const Scalar& color, int thickness, int line_type, int shift, double tipLength);
Size getVelocityInFrame(Rect locationRect, vector<Rect> velVector, Mat& color_current_frame, int* count, int gamePlay);

int main( int argc, const char** argv )
{
	cv::VideoCapture cap;
	Mat color_current_frame;
	Mat current_frame;
	Mat old_frame;
	Mat v;

	Mat IxMat;
	Mat IyMat;
	Mat ItMat;

	//Create gesture tracker
	GestureTrack GT;
	GT.window_size = 20;
	GT.motion_threshold = 1;

	Size gameFrameSize;
	int XDIM;
	int YDIM;
	int score;
	int hasGameStarted;
	Target T;
	srand (time(NULL));

	//Color of motion vector line
	Scalar red = CV_RGB(255,0,0);
	//Frame count; used to ensure we have both current and prior data to use
	int frame_count = 0;
	//Scaling factor; this should be calculated rather than set
	int scaling = 10;

	//Get command line args
	int region = atoi(argv[1]);
	int resolution = atoi(argv[2]);
	int showDerivatives = atoi(argv[3]);

	//Open video stream if possible
	if (argc < 4)
	{
		printf("Usage: region resolution (video)\n");
		exit(1);
	}
	else if(argc > 4)
	{
		cap.open(string(argv[4]));
	}
	else
	{
		cap.open(CV_CAP_ANY);
	}
	if(!cap.isOpened())
	{
		printf("Error: could not load a camera or video.\n");
	}

	//Windows for displaying vector components
	//namedWindow("video", 1);
	//namedWindow("X Motion vectors", 1);
	//namedWindow("Y Motion vectors", 1);
	namedWindow("Motion vectors", 1);

	// Initialise game code
	// Set dimensions equal to window size

	if (showDerivatives==1){
		namedWindow("Ix",1);
		namedWindow("Iy",1);
		namedWindow("It",1);
	}

	//Main loop
	for(;;)
	{
		//Copy current frame to old frame
		old_frame = current_frame.clone();

		//Get new frame and grayscale
		cap >> color_current_frame;
		cvtColor(color_current_frame, current_frame, CV_BGR2GRAY);


		// show images if needed
		if(showDerivatives==1) {
			if(old_frame.data) {
				calcIMatrices(old_frame, current_frame, IxMat, IyMat, ItMat);
				imshow("Ix",IxMat);
				imshow("Iy",IyMat);
				imshow("It",ItMat);
			}
		}
		
		//Initialise Mats
		//x_motion_frame = Mat::zeros(current_frame.rows, current_frame.cols, CV_8UC1);
		//y_motion_frame = Mat::zeros(current_frame.rows, current_frame.cols, CV_8UC1);
		v = Mat::zeros(2, 1, CV_32FC1);

		//Get current frame dimensions
		//Assume old frame has same dimensions
		int nRows = current_frame.rows;
		int nCols = current_frame.cols;
		int xstep, ystep;

		//Vars for holding mean vectors
		float x_sum = 0;
		float y_sum = 0;
		int count = 0;

		// vector of rects for adding all lines together
		vector<Rect> velVector;

		if (frame_count == 0)
		{
			printf("Image dimensions are: (%d, %d)\n", nRows, nCols);
			ystep = nRows / resolution;
			xstep = nCols / resolution;
			printf("Steps are (%d, %d)\n", xstep, ystep);
			
			// Initialise game code
			// Set dimensions equal to window size
			gameFrameSize = color_current_frame.size();
			XDIM = gameFrameSize.height;
			YDIM = gameFrameSize.width;
			T.init(XDIM, YDIM, TARGET_SIZE, BALL_RADIUS);
			hasGameStarted = 0;
			score=0;
		}

		//Loop through images
		if (frame_count > 0 && current_frame.data)
		{
			for (int i=0;i<nCols;i=i+xstep)
			{
				for (int j=0;j<nRows;j=j+ystep)
				{
					//Calc motion vectors
					v = LkTracker(old_frame, current_frame, i, j, region);
					//printf("Vx = %f\n", (scaling*v.at<float>(0, 0)));
					//printf("Vy = %f\n", (scaling*v.at<float>(1, 0)));

					// Add point to velocity vector
					float x_val = v.at<float>(0, 0);
					float y_val = v.at<float>(1, 0);

					//Calibrate noise threshold
					if (frame_count <100)
					{
						GT.calibrate_noise(x_val, y_val);
					}
					else
					{
						//Detect motion vectors above noise threshold
						float noise_threshold = GT.get_threshold();
						if ((x_val * x_val) + (y_val*y_val) > 2*noise_threshold)
						{
							x_sum += x_val;
							y_sum += y_val;
							count++;
							velVector.push_back(Rect(i,j,v.at<float>(0,0),v.at<float>(1,0)));
						}
					}

					//Create start point of motion vector
					Point start = Point(i+xstep/2, j+ystep/2);

					//Create end point of motion vector
					//Point X_end = Point(i+RESOLUTION/2 + (int)(scaling*v.at<float>(0, 0)), j+RESOLUTION/2);
					//Point Y_end = Point(i+RESOLUTION/2, j+RESOLUTION/2 +(int)(scaling*v.at<float>(1, 0)));
					Point end = Point(i+xstep/2+(int)(scaling*v.at<float>(0, 0)), j+ystep/2+(int)(scaling*v.at<float>(1, 0)));

					//Draw motion vectors
					//line(x_motion_frame, start, X_end, red, 1, 8);
					//line(y_motion_frame, start, Y_end, red, 1, 8);
					arrowedLine(color_current_frame, start, end, red, 1, 8, 0, 0.1);

					//Print debugging info
					//printf("start = (%d, %d)\n", start.x, start.y);
					//printf("X end = (%d, %d)\n", X_end.x, X_end.y);
					//printf("Y end = (%d, %d)\n", Y_end.x, Y_end.y);
					//printf("end = (%d, %d)\n", end.x, end.y);

				}
			}
		}

		//printf("Mean vector is (%2.2f, %2.2f)\n", x_sum/count, y_sum/count);

		// get velocity within desired frame

		Rect locationRect = Rect((nCols/4),(nRows/4),nCols/2,nRows/2);
		Size velInRect;
		velInRect = getVelocityInFrame(locationRect, velVector, color_current_frame, &count, hasGameStarted);
		
		x_sum = velInRect.width;
		y_sum=  velInRect.height;
		Point2f mean_vec;

		//Detect Gestures
		if (frame_count == 1)
		{
			printf("Calibrating noise\n");
		}
		else if (frame_count == 99)
		{
			printf("Noise calibrated: threshold = %f\n", GT.get_threshold());
			hasGameStarted = 1;
		}
		else if (frame_count >= 100)
		{

			if (count > 0)
			{
				GT.add_frame(x_sum, y_sum, count);
				mean_vec = GT.detect();
			}

			T.draw_target(color_current_frame);
			T.draw_ball(color_current_frame);
			T.move_ball(-mean_vec.x, mean_vec.y);

			//char* score_string;
			//sprintf(score_string, "Score: %d", score);
			//cv::putText(color_current_frame,score_string,cvPoint(20,20),1,1,cvScalar(255));

			//Restart if we are at the target
			Rect intersec;
			intersec = T.ballRect & T.targetRect;
			//printf("Intersection is (%d, %d), (%d, %d)\n", intersec.x, intersec.y, intersec.width, intersec.height);
			if (intersec.width == 2 * BALL_RADIUS && intersec.height == BALL_RADIUS * 2)
			{
				T.init(XDIM, YDIM, TARGET_SIZE, BALL_RADIUS);
				score++;
			}
		}

		//Break if video has finished
		if(!current_frame.data)
		{
			printf("Error: no frame data.\n");
			break;
		}

		frame_count++;
		//Display motion vectors
		//imshow("X Motion vectors", x_motion_frame);
		//imshow("Y Motion vectors", y_motion_frame);
		//imwrite("v.jpg", v);
		//imshow("Game", game_img);
		imshow("Motion vectors", color_current_frame);
		//imshow("video", current_frame);
		int key;
		key = waitKey(10);
		//printf("%d\n", key);

		// delete velocity vector
		velVector.clear();

		if (frame_count > MAX_FRAME || key == 113)
		{
			cap.release();
			return 1;
		}
	}
}

//Use this main function to test with 3x3 hand made images
/*
int main()
{
	Mat current_frame;
	Mat old_frame;

	old_frame = Mat::zeros(3, 3, CV_8UC1);
	current_frame = Mat::zeros(3, 3, CV_8UC1);

	old_frame.at<uchar>(0, 0) = 1;
	old_frame.at<uchar>(0, 1) = 2;
	old_frame.at<uchar>(0, 2) = 3;
	old_frame.at<uchar>(1, 0) = 0;
	old_frame.at<uchar>(1, 1) = 1;
	old_frame.at<uchar>(1, 2) = 2;
	old_frame.at<uchar>(2, 0) = 0;
	old_frame.at<uchar>(2, 1) = 1;
	old_frame.at<uchar>(2, 2) = 2;

	current_frame.at<uchar>(0, 0) = 0;
	current_frame.at<uchar>(0, 1) = 0;
	current_frame.at<uchar>(0, 2) = 1;
	current_frame.at<uchar>(1, 0) = 0;
	current_frame.at<uchar>(1, 1) = 0;
	current_frame.at<uchar>(1, 2) = 1;
	current_frame.at<uchar>(2, 0) = 0;
	current_frame.at<uchar>(2, 1) = 0;
	current_frame.at<uchar>(2, 2) = 0;

	imwrite("old.jpg", old_frame);

	imwrite("current.jpg", current_frame);


	Mat v;
	v = LkTracker(old_frame, current_frame, 0,0,1);

	printf("Vx = %f, Vy = %f\n", v.at<float>(0, 0), v.at<float>(1, 0));

	return 1;
}
*/

//Calculate X gradient
float Ix(Mat frame_t, Mat frame_t2, int x, int y)
{
	int val1 = (int) (frame_t.at<uchar>(y, x+1) - frame_t.at<uchar>(y, x));
	int val2 = (int) (frame_t.at<uchar>(y+1, x+1) - frame_t.at<uchar>(y+1, x));
	int val3 = (int) (frame_t2.at<uchar>(y, x+1) - frame_t2.at<uchar>(y, x));
	int val4 = (int) (frame_t2.at<uchar>(y+1, x+1) - frame_t2.at<uchar>(y+1, x));

	//printf("Ix = %f\n", (((float)(val1 + val2 + val3 + val4) / 4)));
	return ((float)(val1 + val2 + val3 + val4) / 4);
}

//Calculate Y gradient
float Iy(Mat frame_t, Mat frame_t2, int x, int y)
{
	int val1 = (int) (frame_t.at<uchar>(y+1, x) - frame_t.at<uchar>(y, x));
	int val2 = (int) (frame_t.at<uchar>(y+1, x+1) - frame_t.at<uchar>(y, x+1));
	int val3 = (int) (frame_t2.at<uchar>(y+1, x) - frame_t2.at<uchar>(y, x));
	int val4 = (int) (frame_t2.at<uchar>(y+1, x+1) - frame_t2.at<uchar>(y, x+1));

	//printf("Iy = %f\n", (((float)(val1 + val2 + val3 + val4) / 4)));
	return ((float)(val1 + val2 + val3 + val4) / 4);	
}

//Calcualte temporal gradient
float It(Mat frame_t, Mat frame_t2, int x, int y)
{
	int val1 = (int) (frame_t2.at<uchar>(y, x) - frame_t.at<uchar>(y, x));
	int val2 = (int) (frame_t2.at<uchar>(y+1, x) - frame_t.at<uchar>(y+1, x));
	int val3 = (int) (frame_t2.at<uchar>(y, x+1) - frame_t.at<uchar>(y, x+1));
	int val4 = (int) (frame_t2.at<uchar>(y+1, x+1) - frame_t.at<uchar>(y+1, x+1));

	//printf("It = %f\n", (((float)(val1 + val2 + val3 + val4) / 4)));
	return ((float)(val1 + val2 + val3 + val4) / 4);
}

//Calculate motion vectors
Mat LkTracker(Mat frame_t, Mat frame_t2, int x, int y, int size)
{
	//Create vars
	float IxIx = 0;
	float IyIy = 0;
	float IxIy = 0;
	float IxIt = 0;
	float IyIt = 0;

	Mat A, B, v, padded_frame_t, padded_frame_t2;
	A =  Mat::zeros(2, 2, CV_32FC1);
	B = Mat::zeros(2, 1, CV_32FC1);
	v = Mat::zeros(2, 1, CV_32FC1);

	//Add border to frames
	cv::copyMakeBorder( frame_t, padded_frame_t, 0, size+1, 0, size+1 ,cv::BORDER_REPLICATE);
	cv::copyMakeBorder( frame_t2, padded_frame_t2, 0, size+1, 0, size+1 ,cv::BORDER_REPLICATE);

	//Loop through ROI
	for (int i = 0; i<= size; i++)
	{
		for (int j = 0; j<= size; j++)
		{
			//Accumulate Matrix values
			float Ix_val = Ix(padded_frame_t, padded_frame_t2, x+i, y+j);
			float Iy_val = Iy(padded_frame_t, padded_frame_t2, x+i, y+j);
			float It_val = It(padded_frame_t, padded_frame_t2, x+i, y+j);

			IxIx += (Ix_val * Ix_val);	
			IyIy += (Iy_val * Iy_val);
			IxIy += (Ix_val * Iy_val);
			IxIt -= (Ix_val * It_val);
			IyIt -= (Iy_val * It_val);
		}
	}

	//Check for singular matrices
	if ((IxIx * IyIy) - (IxIx*IxIx) == 0) 
	{
		//printf("Singular\n");
		v.at<float>(0, 0) = 0;
		v.at<float>(1, 0) = 0;
	}
	else
	{
		//Solve for motion vectors
		A.at<float>(0, 0) = IxIx;
		A.at<float>(0, 1) = IxIy;
		A.at<float>(1, 0) = IxIy;
		A.at<float>(1, 1) = IyIy;

		B.at<float>(0, 0) = IxIt;
		B.at<float>(1, 0) = IyIt;

		v = (A.inv()*B);

		// debug statements
		//Debugging info.
		//printf("A11 = %f\n", A.at<float>(0, 0));
		//printf("A12 = %f\n", A.at<float>(0, 1));	
		//printf("A21 = %f\n", A.at<float>(1, 0));
		//printf("A22 = %f\n", A.at<float>(1, 1));

		//printf("B1 = %f\n", B.at<float>(0, 0));
		//printf("B2 = %f\n", B.at<float>(1, 0));

		//printf("Vx = %f\n", v.at<float>(0, 0));
		//printf("Vy = %f\n", v.at<float>(1, 0));

		//Mat temp;
		//temp = A.inv();

		//printf("A11 = %f\n", temp.at<float>(0, 0));
		//printf("A12 = %f\n", temp.at<float>(0, 1));	
		//printf("A21 = %f\n", temp.at<float>(1, 0));
		//printf("A22 = %f\n", temp.at<float>(1, 1));

		//imwrite("a.jpg", A);
		//imwrite("a_inv.jpg", temp);
		//imwrite("B.jpg", B);
		//imwrite("v.jpg", v);
	}

	return v;
}

//Draw an arrowedLine instead of an arrow
//Shamelessly stolen from stackoverflow
static void arrowedLine(Mat img, Point pt1, Point pt2, const Scalar& color, int thickness=1, int line_type=8, int shift=0, double tipLength=0.1)
{
    const double tipSize = norm(pt1-pt2)*tipLength; // Factor to normalize the size of the tip depending on the length of the arrow
    line(img, pt1, pt2, color, thickness, line_type);
    const double angle = atan2( (double) pt1.y - pt2.y, (double) pt1.x - pt2.x );
    Point p(cvRound(pt2.x + tipSize * cos(angle + CV_PI / 4)),
    cvRound(pt2.y + tipSize * sin(angle + CV_PI / 4)));
    line(img, p, pt2, color, thickness, line_type);
    p.x = cvRound(pt2.x + tipSize * cos(angle - CV_PI / 4));
    p.y = cvRound(pt2.y + tipSize * sin(angle - CV_PI / 4));
    line(img, p, pt2, color, thickness, line_type);
}


Size getVelocityInFrame(Rect locationRect, vector<Rect> velVector, Mat& color_current_frame, int* count, int gamePlay){
	
	// Initialise size of velocity to return (default 0)
	Size retVel = Size(0,0);
	float x_val = 0.0;
	float y_val = 0.0;

	// draw vector onto image
	if (gamePlay==1)
	{
		rectangle(color_current_frame, locationRect, CV_RGB(0,255,0),1,8,0);
	}
	else
	{
		rectangle(color_current_frame, locationRect, CV_RGB(255,0,0),1,8,0);
	}

	//Reset counter to zero
	*count = 0;
	// cycle through velocities
	for (int i=0; i<velVector.size(); i++){
		// check if within rectangle
		if(velVector.at(i).x > locationRect.x && velVector.at(i).x < locationRect.x+locationRect.width) {
			if (velVector.at(i).y > locationRect.y && velVector.at(i).y < locationRect.y+locationRect.height) {

				// Point is within rectangle, use velocity value
				x_val+=velVector.at(i).width;
				y_val+=velVector.at(i).height;
				//Increment count
				(*count)++;
			}
		}
	}

	//Return values
	retVel.width = x_val;
	retVel.height = y_val;

	return(retVel);
}