

#include "stdafx.h"
#include <iostream>

#include <opencv2\opencv.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

using namespace std;
using namespace cv;


int main()
{
	Mat img = imread(".\\image\\百事可乐.jpg", IMREAD_GRAYSCALE);
	Mat out;
	Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));

	dilate(img, out, element);

	int channels = 0;
	MatND dstHist;
	int histSize[] = { 256 };
	float midRanges[] = { 0, 256 };
	const float *ranges[] = { midRanges };
	
	calcHist(&img, 1, &channels, Mat(), dstHist, 1, histSize, ranges, true, false);
	
	Mat drawImage = Mat::zeros(Size(256, 256), CV_8UC1);
	
	double g_dHistMaxValue;
	minMaxLoc(dstHist, 0, &g_dHistMaxValue, 0, 0);
	for (int i = 0; i < 256; i++)
	{
		int value = cvRound(dstHist.at<float>(i) * 256 * 0.9 / g_dHistMaxValue);
		cout << dstHist.at<float>(i) << endl;
		line(drawImage, Point(i, drawImage.rows - 1), Point(i, drawImage.rows - 1 - value), 255);
	}

	imshow("hist", drawImage);
	waitKey(0);


	threshold(img, img, 100, 255, THRESH_BINARY);
	imshow("img", img);
	//waitKey(0);
	vector<vector<Point>> contours;
	//Mat contours(img.rows, img.cols, CV_8UC1);
	findContours(img, contours, noArray(), RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	

	img = Scalar::all(0);
	drawContours(img, contours,-1, 125);

	imshow("contours", img);
	waitKey(0);
}
