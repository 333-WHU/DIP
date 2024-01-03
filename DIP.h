#pragma once
#include <iostream>
#include<string>
#include <opencv2/core/core.hpp>   
#include <opencv2/highgui/highgui.hpp>


using namespace std;  // 省去屏幕输出函数cout前的std::
using namespace cv;   // 省去opencv函数前面加cv::的必要性
class DIP {
	public:
	//读取彩色图
	void pre_process_color(string ImageName);
	//读取灰度图
	void pre_process_pan(string ImageName);
	//灰度图中值滤波
	Mat Median_Filtering ()const;
	//彩色图%2线性拉伸
	Mat Linear_stretching()const;
	//彩色图中值滤波
	Mat Median_Filtering_color()const;
	//彩色图平移
	Mat Translation();
	//灰度图二值化 
	Mat binary();
	//灰度图的高通滤波
	Mat High_pass_filtering()const;
	//灰度图低通滤波
	Mat Low_pass_filtering()const;
private:
	//读取的图像
	Mat bmp;
	//读取图像的类型
	bool b_color;
	//扩充的图像
	Mat* bmp_padding;
	//现在的图像的
};

