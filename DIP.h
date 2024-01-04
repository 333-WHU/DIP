#pragma once
#include <iostream>
#include<string>
#include <opencv2/core/core.hpp>   
#include <opencv2/highgui/highgui.hpp>
#include"Frequency_distribution.h"

using namespace std;  // 省去屏幕输出函数cout前的std::
using namespace cv;   // 省去opencv函数前面加cv::的必要性

class DIP {
public:
	DIP();
	//读取文件，默认为灰度图，彩色图b_color=1
	DIP(string ImageName, bool b_color1=0);
	//读取彩色影像图,t
	void pre_process_color(string ImageName);
	//读取灰度影像图,t
	void pre_process_pan(string ImageName);
	//灰度图中值滤波,t
	Mat Median_Filtering ()const;
	//彩色图%2线性拉伸
	Mat Linear_stretching()const;
	//灰度图线性拉伸
	Mat Linear_stretching_gray()const;
	//彩色图中值滤波
	Mat Median_Filtering_color()const;
	//灰度图的高通滤波,t
	Mat High_pass_filtering()const;
	//灰度图低通滤波,t
	Mat Low_pass_filtering()const;
	//灰度图二值化,状态法,t
	Mat binary(uchar T);
	//灰度图二值化,判断分析法,t
	Mat binary2(uchar T);
	//复制一份图像
	Mat Getbmp();
private:
	//读取的图像
	Mat bmp;
	//读取图像的类型
	bool b_color;
};

void test();