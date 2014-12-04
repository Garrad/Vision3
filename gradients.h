#ifndef GRADIENTS_H_
#define GRADIENTS_H_

using namespace cv;
using namespace std;

void calcIMatrices(Mat prevFrame, Mat currFrame, Mat &Ix, Mat &Iy, Mat &It);

#endif