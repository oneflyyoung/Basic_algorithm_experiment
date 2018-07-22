
#include "stdafx.h"
#include <iostream>

#include <opencv2\opencv.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

using namespace std;
using namespace cv;

int main()
{
	Mat srcImage = imread("D:\\DOC\\test.tif", 2);

	int channels = 0;
	MatND dstHist;
	int histSize[] = { 1000 };
	float midRanges[] = { 0, 1000 };
	const float *ranges[] = { midRanges };

	//double time = (double)cvGetTickCount();

	calcHist(&srcImage, 1, &channels, Mat(), dstHist, 1, histSize, ranges, true, false);

	//Mat drawImage = Mat::zeros(Size(256, 256), CV_16UC3);

	//double g_dHistMaxValue;
	//minMaxLoc(dstHist, 0, &g_dHistMaxValue, 0, 0);
	int count = 0;
	for (int i = 0; i < 1000; i++)
	{
		////int value = cvRound(dstHist.at<float>(i) * 256 * 0.9 / g_dHistMaxValue);
		if (dstHist.at<float>(i) != 0) 
		{
			cout << i << "   " << dstHist.at<float>(i) << endl; 
			count++;
		}
		//line(drawImage, Point(i, drawImage.rows - 1), Point(i, drawImage.rows - 1 - value), Scalar(0, 0, 255));
	}
	cout << count;
	//time = (double)cvGetTickCount() - time;
	//imshow("hist", drawImage);
	waitKey(0);

	//cout << time / (cvGetTickFrequency() * 1000);
	getchar();
	return 0;
}