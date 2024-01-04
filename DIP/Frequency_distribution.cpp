#include "Frequency_distribution.h"

Frequency_distribution::Frequency_distribution(Mat bmp, bool b_color):distribution_color(3, vector<int>(256, 0)),distribution_gray(256,0)
, cumulative_histogram_color(3, vector<int>(256, 0)),cumulative_histogram_gray(256,0)
{   
    if(b_color){
        numimg = bmp.cols * bmp.rows;
        for (int i = 0; i < bmp.rows; i++) {
            for (int j = 0; j < bmp.cols; j++) {
                for (int channel = 0; channel < bmp.channels(); channel++) {
                    distribution_color[channel][bmp.at<Vec3b>(i, j)[channel]] += 1;
                }
            }
        }
        for (int i = 0; i < 256; i++) {
            for (int j = 0; j < i + 1; j++) {
                for(int channel = 0; channel < bmp.channels(); channel++) {
                    cumulative_histogram_color[channel][i] += distribution_color[channel][j];
                }
            }
        }
    }
    else {
        numimg = bmp.cols * bmp.rows;
        for (int i = 0; i < bmp.rows; i++) {
            for (int j = 0; j < bmp.cols; j++) {
                distribution_gray[bmp.at<uchar>(i, j)] += 1;
            }
        }
        for (int i = 0; i < 256; i++) {
            for (int j = 0; j < i + 1; j++) {
                    cumulative_histogram_gray[i] += distribution_gray[j];
            }
        }
        expectation = 0;
        int t = 0;
        var = 0;
        for (auto it = distribution_gray.begin(); it != distribution_gray.end(); it++, t++) {
            expectation += (double)*it * t;
        }
        expectation /= numimg;
        for (auto it = distribution_gray.begin(); it != distribution_gray.end(); it++, t++) {
            var += *it * (t - expectation) * (t - expectation);
        }
        var /= numimg;
    }
}

double Frequency_distribution::var_ratio(uchar T)
{
    double b = var_int(distribution_gray, 0, T) / var_int(distribution_gray, T, 256);
    return var_int(distribution_gray, 0, T) / var_int(distribution_gray, T, 256);
}

double var_int(vector<int> dis, int i,int j)
{
    double expectation = 0;
    double e2 = 0.0;
    double var1,var = 0.0;
    int n = 0;//像素总个数
    for (int m = i; m < j;m++) {
        expectation += dis[m] * m ;
        //e2 +=  dis[m] * m*m ;
        n += dis[m];
    }
    expectation /= n;
    //e2 /= n;
    for (int m = i; m < j; m++) {
        var += dis[m] * (m - expectation) * (m - expectation);
    }
    var /= n;
    //var1 = e2 - expectation * expectation;
    return var;
}
