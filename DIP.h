#pragma once
#include <iostream>
#include<string>
#include <opencv2/core/core.hpp>   
#include <opencv2/highgui/highgui.hpp>


using namespace std;  // ʡȥ��Ļ�������coutǰ��std::
using namespace cv;   // ʡȥopencv����ǰ���cv::�ı�Ҫ��
class DIP {
	public:
	//��ȡ��ɫͼ
	void pre_process_color(string ImageName);
	//��ȡ�Ҷ�ͼ
	void pre_process_pan(string ImageName);
	//�Ҷ�ͼ��ֵ�˲�
	Mat Median_Filtering ()const;
	//��ɫͼ%2��������
	Mat Linear_stretching()const;
	//��ɫͼ��ֵ�˲�
	Mat Median_Filtering_color()const;
	//��ɫͼƽ��
	Mat Translation();
	//�Ҷ�ͼ��ֵ�� 
	Mat binary();
	//�Ҷ�ͼ�ĸ�ͨ�˲�
	Mat High_pass_filtering()const;
	//�Ҷ�ͼ��ͨ�˲�
	Mat Low_pass_filtering()const;
private:
	//��ȡ��ͼ��
	Mat bmp;
	//��ȡͼ�������
	bool b_color;
	//�����ͼ��
	Mat* bmp_padding;
	//���ڵ�ͼ���
};

