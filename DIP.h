#pragma once
#include <iostream>
#include<string>
#include <opencv2/core/core.hpp>   
#include <opencv2/highgui/highgui.hpp>


using namespace std;  // 省去屏幕输出函数cout前的std::
using namespace cv;   // 省去opencv函数前面加cv::的必要性
//彩色图
Mat pre_process_color(string ImageName);
//灰度图
Mat pre_process_pan(string ImageName);
//灰度图中值滤波
Mat Median_Filtering(string ImageName);
//彩色图%2线性拉伸
Mat Linear_stretching(string ImageName);
