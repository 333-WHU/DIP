#include <iostream>
#include<string>
#include <opencv2/core/core.hpp>   
#include <opencv2/highgui/highgui.hpp>

using namespace std;  // 省去屏幕输出函数cout前的std::
using namespace cv;   // 省去opencv函数前面加cv::的必要性
class Frequency_distribution
{
public:
	//由图像生成的直方图
	Frequency_distribution(Mat bmp,bool b_color = 0);
	//给定一个uchar T，将返回由它所分割的两部分的方差比值
	double var_ratio(uchar T);
private:
	vector<int> distribution_gray;
	vector<int> cumulative_histogram_gray;
	vector<vector<int>> distribution_color;
	vector<vector<int>>cumulative_histogram_color;
	int numimg;//像素个数
	/*     方差和期望只应用于灰度图      */
	double var;//方差
	double expectation;//期望

};
//对int数组算方差，a[i]为i的个数,从i到j（不包括a[j]）
double var_int(vector<int>dis, int i,int j);
