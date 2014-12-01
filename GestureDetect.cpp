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

class GestureDetect
{
	private:
		int window_size = 0;
		int window_max = 100;
	public:
		vector<Mat> window;

		void add_frame(Mat frame)
		{
			if (window_size < window_max)
			{
				window.push_back(frame);
			}
			else
			{
				window.erase(window.begin());
				window.push_back(frame);	
			}
		}

		void flush(char* tag)
		{
			
		}

		void train()
		{
			char temp;
			char count = 0;
			while (scanf(" %c", &temp))
			{
				if (temp == 'q') 
				{
					return;
				}
				printf("Training\n");

				cv::VideoCapture cap;
				cap.open(CV_CAP_ANY);

				if(!cap.isOpened())
				{
					printf("Error: could not load a camera or video.\n");
					return;
				}

				Mat frame, grey_frame;
				int frame_count = 0;

				for (int i = 0;i<window_size;i++)
				{
					cap >> frame;
					cvtColor(frame, grey_frame, CV_BGR2GRAY);
					add_frame(frame);
				}

				char tag[100];
				printf("Please enter gesture name\n");
				scanf("%s", tag);
			}
		}
};

int main()
{
	GestureDetect GD;
	GD.train();
	return 1;
}