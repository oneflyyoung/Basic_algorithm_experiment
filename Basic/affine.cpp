
#include "stdafx.h"
#include <iostream>

#include <opencv2\opencv.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

using namespace std;
using namespace cv;

int main()
{
	Mat img = imread("image\\50_60Hz.jpg");
	imshow("origin", img);
	double angle = 45;
	Size2f src_sz = img.size();
	float new_width = abs(src_sz.width*sin(angle)) + abs(src_sz.height*cos(angle));
	float new_height = abs(src_sz.width*cos(angle)) + abs(src_sz.height*sin(angle));
	//Size2f dst_sz(src_sz.width, src_sz.height);
	Size2f dst_sz(new_width, new_height);
	//float len = max(img.rows, img.cols);
	Point2f center(src_sz.width / 2., src_sz.height / 2.);
	//float scale = src_sz.height / new_height < src_sz.width / new_width ? src_sz.height / new_height : src_sz.width / new_width;
	Mat rotMat = getRotationMatrix2D(center, angle, 1);
	//cout << rotMat.type();
	Mat dst(dst_sz.width, dst_sz.height, CV_8UC3);
	warpAffine(img, dst, rotMat, dst_sz);
	imshow("test", dst);
	imwrite("0000000000.jpg", dst);
	waitKey(0);
}