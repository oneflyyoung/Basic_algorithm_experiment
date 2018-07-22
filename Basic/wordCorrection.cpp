#include "stdafx.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <opencv2\opencv.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

#include <Windows.h>

using namespace std;
using namespace cv;

string path = "image\\";

string UTF8ToGB(const char* str)
{
	string result;
	WCHAR *strSrc;
	LPSTR szRes;

	//获得临时变量的大小
	int i = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	strSrc = new WCHAR[i + 1];
	MultiByteToWideChar(CP_UTF8, 0, str, -1, strSrc, i);

	//获得临时变量的大小
	i = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
	szRes = new CHAR[i + 1];
	WideCharToMultiByte(CP_ACP, 0, strSrc, -1, szRes, i, NULL, NULL);

	result = szRes;
	delete[]strSrc;
	delete[]szRes;

	return result;
}

vector<string> split(string str, const char ch)
{
	vector<string> result;
	string temp("");

	for (int i = 0; i < str.length(); ++i)
	{
		if (str[i] == ch)
		{
			result.push_back(temp);
			temp = "";
		}
		else
			temp += str[i];
	}
	result.push_back(temp);

	return result;
}

int main()
{
	cv::Mat img = imread("test4.jpg");

	ifstream fin("test4.txt", ios::binary);
	char str[100];
	while (fin.peek() != EOF)
	{
		fin.getline(str, 100);
		string msg = UTF8ToGB(str);
		vector<string> temp = split(msg, ',');
		vector<Point2f> p;
		p.push_back(Point2f(atof(temp[0].c_str()), atof(temp[1].c_str())));
		p.push_back(Point2f(atof(temp[2].c_str()), atof(temp[3].c_str())));
		p.push_back(Point2f(atof(temp[4].c_str()), atof(temp[5].c_str())));
		p.push_back(Point2f(atof(temp[6].c_str()), atof(temp[7].c_str())));
		string labelName = temp[8];

		if (labelName != "###")
		{

			//求取最小外接矩形
			float y_min = 10000, x_min = 10000, y_max = 0, x_max = 0;
			for (int i = 0; i < 4; ++i)
			{
				if (p[i].x > x_max) x_max = p[i].x;
				if (p[i].y > y_max) y_max = p[i].y;
				if (p[i].x < x_min) x_min = p[i].x;
				if (p[i].y < y_min) y_min = p[i].y;
			}
			/*line(img, Point(x_min, y_min), Point(x_max, y_min), Scalar(255));
			line(img, Point(x_max, y_min), Point(x_max, y_max), Scalar(255));
			line(img, Point(x_max, y_max), Point(x_min, y_max), Scalar(255));
			line(img, Point(x_min, y_max), Point(x_min, y_min), Scalar(255));*/
			/*line(img, p[0], p[1], Scalar(255));
			line(img, p[1], p[2], Scalar(255));
			line(img, p[2], p[3], Scalar(255));
			line(img, p[3], p[0], Scalar(255));
			imshow("img", img);
			waitKey(0)*/;

			//裁切初始定位点最小外接矩形
			Mat roi1 = img(Range(floor(y_min), ceil(y_max)), Range(floor(x_min), ceil(x_max)));
			Mat dst1((y_max - y_min), (x_max - x_min), CV_8UC3);
			roi1.copyTo(dst1);//  dst1为最小外接矩形包裹文字的矩形
			//裁切后定位点坐标变换
			for (int i = 0; i < 4; ++i)
			{
				p[i].x = p[i].x - x_min;
				p[i].y = p[i].y - y_min;
			}

			/*line(dst1, p[0], p[1], Scalar(255));
			line(dst1, p[1], p[2], Scalar(255));
			line(dst1, p[2], p[3], Scalar(255));
			line(dst1, p[3], p[0], Scalar(255));
			imshow("seg", dst1);
			waitKey(0);*/

			//计算旋转角度
			double angle;
			if (pow(p[0].x - p[1].x, 2) + pow(p[0].y - p[1].y, 2) > pow(p[2].x - p[1].x, 2) + pow(p[2].y - p[1].y, 2))
				angle = atan2(p[1].y - p[0].y, p[1].x - p[0].x);
			else
				angle = atan2(p[2].y - p[1].y, p[2].x - p[1].x);
			angle = angle * 180.0 / CV_PI;

			//文字接近水平或垂直不进行旋转
			if (abs(abs(angle) - 90) < 10 || abs(angle) < 10)
			{
				vector<string> name = split(labelName, '/');
				string filename = path;
				for (int i = 0; i < name.size(); ++i) filename += name[i] + '_';
				filename.pop_back();
				filename += ".jpg";
				imwrite(filename, dst1);

				cout << labelName << " done" << endl;
			}
			else
			{
				//将第一次裁切所得图片旋转，文字摆正
				Size2f src_sz = dst1.size();
				float new_height = abs(src_sz.width*sin(angle)) + abs(src_sz.height*cos(angle));
				float new_width = abs(src_sz.width*cos(angle)) + abs(src_sz.height*sin(angle));
				Size2f dst_sz(src_sz.height, src_sz.width);
				float len = max(dst1.rows, dst1.cols);
				Point2f center(len / 2., len / 2.);
				float scale = src_sz.height / new_height > src_sz.width / new_width ? src_sz.height / new_height : src_sz.width / new_width;
				Mat rotMat = getRotationMatrix2D(center, angle, scale);
				//cout << rotMat.type();
				Mat dst2(dst_sz.height, dst_sz.width, CV_32FC1);
				warpAffine(dst1, dst2, rotMat, dst_sz);  //dst2为摆正文字之后的图片
				imshow("test", dst2);
				waitKey(0);

				//修正定位点坐标
				rotMat.convertTo(rotMat, CV_32FC1);
				for (int i = 0; i < 4; ++i)
				{
					float x = p[i].x - center.x;
					float y = p[i].y - center.y;
					/*Point temp;
					temp.x = round(x*cos(angle) + y*sin(angle))*scale + center.x;
					temp.y = round(-x*sin(angle) + y*cos(angle))*scale + center.y;*/
					//float alpha = scale*cos(angle);
					//float beta = scale*sin(angle);
					//temp.x = round(x*cos(angle) + y*sin(angle))*scale + (1 - alpha)*center.x - beta*center.y;
					//temp.y = round(-x*sin(angle) + y*cos(angle))*scale + beta*center.x + (1 - alpha)*center.y;
					//p[i] = temp;

					Mat vec(3, 1, CV_32FC1);
					vec.at<float>(0, 0) = p[i].x;
					vec.at<float>(1, 0) = p[i].y;
					vec.at<float>(2, 0) = 1.00f;
					/*Mat vec(1,2, CV_32FC1);
					vec.at<float>(0, 0) = x;
					vec.at<float>(0, 1) = y;*/
					Mat temp = rotMat*vec;

					p[i].x = temp.at<float>(0, 0);// +center.x;
					p[i].y = temp.at<float>(1, 0);// +center.y;
				}
				/*line(dst2, p[0], p[1], Scalar(255), 4);
				line(dst2, p[1], p[2], Scalar(255), 4);
				line(dst2, p[2], p[3], Scalar(255), 4);
				line(dst2, p[3], p[0], Scalar(255), 4);*/

				/*imshow("rot", dst2);
				waitKey(0);*/

				y_min = 10000, x_min = 10000, y_max = 0, x_max = 0;
				for (int i = 0; i < 4; ++i)
				{
					if (p[i].x > x_max) x_max = p[i].x;
					if (p[i].y > y_max) y_max = p[i].y;
					if (p[i].x < x_min) x_min = p[i].x;
					if (p[i].y < y_min) y_min = p[i].y;
				}
				if (x_min < 0) x_min = 0;
				if (y_min < 0) y_min = 0;
				if (x_max > dst2.cols) x_max = dst2.cols;
				if (y_max > dst2.rows) y_max = dst2.rows;
				Mat roi3 = dst2(Range(y_min, y_max), Range(x_min, x_max));
				Mat dst3((y_max - y_min), (x_max - x_min), CV_32FC1);
				roi3.copyTo(dst3);

				string filename = path + labelName + ".jpg";
				imwrite(filename, dst3);

				//findContours()
			}
		}
	}
	fin.close();

	getchar();
    return 0;
}