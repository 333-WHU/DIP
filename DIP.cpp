#include "DIP.h"

void DIP::pre_process_color(string ImageName)
{
    //landsat的bgr分别是234波段，全色是第8波段
    Mat mss = imread(ImageName, IMREAD_UNCHANGED);   // 读入图片 

    if (mss.empty())     // 判断文件是否正常打开  
    {
        cout<< "Can not load image "<<ImageName<<"\n";
        waitKey(6000);  // 等待6000 ms后窗口自动关闭   
    }
    cout << ImageName << ":" << endl;
    cout << "depth = " << mss.depth() << endl << "channels = " << mss.channels() << endl;
    cout << "Number of rows = " << mss.rows << endl << "Number of columns = " << mss.cols << endl;
    cout << "Dimension = " << mss.dims << endl << "Number of bytes per element = " << mss.elemSize() << endl;
    cout << "Number of bytes per channel per element = " << mss.elemSize1() << endl << "type = " << mss.type() << endl;


    Mat b_g_r_ir[4], color_arr[3];
    split(mss, b_g_r_ir);

    color_arr[0] = b_g_r_ir[2];
    color_arr[1] = b_g_r_ir[1];
    color_arr[2] = b_g_r_ir[0];
    merge(color_arr, 3, bmp);

    //各波段数值范围大概在0-1001之间，所以建议转至8比特时，除以4
    bmp.convertTo(bmp, CV_8UC3, 1.0 / 4.0, 0);

    namedWindow("color", WINDOW_NORMAL);  //若图片太大，用WINDOW_NORMAL的方式可以让图片的显示大小随窗口大小缩放
    imshow("color", bmp);  // 显示图片 
    waitKey();

    //若仅需要裁剪部分做后续处理
    //Mat subset = color(Range(2650, 3150), Range(5400, 5900)); //subset共享color的部分数据，前面为行范围，后面为列范围
    //imwrite("mss_subset.bmp", subset); //16U的数据可以保存至png、jpeg2000或tiff文件，其中png和jpeg2000需要设置压缩参数前面为行范围，后面为列范围
}

void DIP::pre_process_pan(string ImageName)
{
    bmp = imread(ImageName, IMREAD_UNCHANGED);    //读入全色波段
    cout << ImageName << ":" << endl;
    cout << "depth = " << bmp.depth() << endl << "channels = " << bmp.channels() << endl;
    cout << "Number of rows = " << bmp.rows << endl << "Number of columns = " << bmp.cols << endl;
    cout << "Dimension = " << bmp.dims << endl << "Number of bytes per element = " << bmp.elemSize() << endl;
    cout << "Number of bytes per channel per element = " << bmp.elemSize1() << endl << "type = " << bmp.type() << endl;

    //最大值552， 除以4
    bmp.convertTo(bmp, CV_8UC1, 1.0 / 4.0, 0);

    namedWindow("pan", WINDOW_NORMAL);  //若图片太大，用WINDOW_NORMAL的方式可以让图片的显示大小随窗口大小缩放
    imshow("pan", bmp);  // 显示图片 
    waitKey();

}

Mat DIP::Median_Filtering() const {
    Mat bmp_filted;
    bmp_filted.create(bmp.rows, bmp.cols, CV_8UC1);

    unsigned char* pBmp_filted = bmp_filted.data;
    int size = 3;//窗口大小
    uchar* pTmp = new uchar[size * size];//缓冲区
    for (int i = size / 2; i < bmp_filted.rows - size / 2; i++)//从第size/2开始到倒数第size/2列
    {
        for (int j = size / 2; j < bmp_filted.cols - size / 2; j++)//从第size/2开始到倒数第size/2行
        {   //这个是以先代值再排序的方法
            // data
            int idx = 0;
            for (int k = i - size / 2; k <= i + size / 2; k++)
            {
                for (int l = j - size / 2; l <= j + size / 2; l++)
                {
                    pTmp[idx++] = *bmp.ptr<uchar>(k, l);
                }
            }
            //sorting
            for (int k = 0; k < size * size - 1; k++)
            {
                int idx = k, tdata;
                for (int l = k + 1; l < size * size; l++)
                {
                    if (pTmp[l] < pTmp[idx]) idx = l;
                }
                if (idx != k)
                {
                    tdata = pTmp[idx];
                    pTmp[idx] = pTmp[k];
                    pTmp[k] = tdata;
                }
            }
            ////这个是以栈作为排序的方法
            //pTmp[0] = *bmp.ptr<uchar>(i - size / 2, j - size / 2);//栈的最大长度为size*size，第一个元素直接入栈，以从小到大入栈
            //int length = 1;//当前栈的长度
            //for (int k = i - size / 2 + 1; k <= i + size / 2; k++){
            //    for (int l = j - size / 2; l <= j + size / 2; l++)
            //    {   
            //        int t = 0;
            //        while(t<length){
            //            if (*bmp.ptr<uchar>(k, l) > pTmp[t]) {
            //                t++;
            //            }
            //            else {
            //                for (int ik = length; ik > t; ik--) {
            //                    pTmp[ik] = pTmp[ik - 1];
            //                }
            //                pTmp[t] = *bmp.ptr<uchar>(k, l);
            //            }//如果小于当前元素，但是大于之前的元素，则当前元素之后（包括当前）的元素出栈（到后一个）
            //        }
            //        if (t == length) {
            //            pTmp[length] = *bmp.ptr<uchar>(k, l);
            //        }//比所有元素大，入栈
            //        length++;//入栈结束栈的长度加一
            //    }
            //}
            //assignment
            pBmp_filted[i * bmp_filted.cols + j] = pTmp[(size * size) / 2];
        }
    }

    namedWindow("display");
    imshow("display", bmp_filted);
    waitKey(0);

    imwrite("bmp_filted.bmp", bmp_filted);
    return bmp_filted;

}

//拉伸的处理函数,将t在a到b范围内拉伸
uchar Linear_stretching1(uchar t,int a, int b) {
    if (t < a) {
        t = 0;
    }
    else {
        if (t > b){
            t = 255;
        }
        else {
            t = (uchar)b / (b - a) * (t - a);
        }
    }
    return t;
}

Mat DIP::Linear_stretching()const
{
    namedWindow("color", WINDOW_NORMAL);  //若图片太大，用WINDOW_NORMAL的方式可以让图片的显示大小随窗口大小缩放
    imshow("color", bmp);  // 显示图片 
    waitKey();
    int vgray[256][3] = { 0 };//灰度频率直方图
    int value = 2;//线性拉伸的程度
    int numa = (int)bmp.rows * bmp.cols * value / 100;//初始的像素频率
    int numb = (int)bmp.rows * bmp.cols * (100 - value) / 100;//最终的像素频率
    int c = 0, d = 255;//目标
    int a[3] = { 0 }, b[3] = { 255 }; // 左值数组和右值数组
    for (int i = 0; i < bmp.rows; i++) {
        for (int j = 0; j < bmp.cols; j++) {
           for (int channel = 0; channel < bmp.channels(); channel++) {
             vgray[bmp.at<Vec3b>(i,j)[channel]][channel]+=1;
            }
        }
    }
    //计算左值
     for(int channel=0;channel<bmp.channels();channel++){
        for (int i = 0,sum[3] = { 0 }; i < 256; i++){
            sum[channel] += vgray[i][channel];
            if (sum[channel] > numa) {
                a[channel] = i;
                break;
            }
        }
     }
     //计算右值
     for (int channel = 0; channel < bmp.channels(); channel++) {
         for (int i = 255,sum[3] = { 0 }; i >=0; i--) {
             sum[channel] += vgray[i][channel];
             if (sum[channel] > numa) {
                 b[channel] = i;
                 break;
             }
         }
     }
     //拉伸,新图为bmpnew
     Mat bmpnew = bmp.clone();
     for (int i = 0; i < bmp.rows; i++) {
         for (int j = 0; j < bmp.cols; j++) {
             for (int channel = 0; channel < bmp.channels(); channel++) {
                 //处理函数
                 bmpnew.at<Vec3b>(i,j)[channel] = Linear_stretching1(bmp.at<Vec3b>(i, j)[channel], a[channel], b[channel]);
             }
         }
     }
     namedWindow("Linear_stretching", WINDOW_NORMAL);  //若图片太大，用WINDOW_NORMAL的方式可以让图片的显示大小随窗口大小缩放
     imshow("Linear_stretching",bmpnew);// 显示图片 
     waitKey();
     
     return bmpnew;
}

Mat DIP::Median_Filtering_color()const
{
    Mat bmp_filted = bmp.clone();
    int size = 3;//窗口大小
    uchar* pTmp = new uchar[size * size];//缓冲区
    for(int channel=0;channel<bmp.channels();channel++){
        for (int i = size / 2; i < bmp_filted.rows - size / 2; i++)//从第size/2开始到倒数第size/2列
        {
            for (int j = size / 2; j < bmp_filted.cols - size / 2; j++)//从第size/2开始到倒数第size/2行
            {   //这个是以先代值再排序的方法
                // data
                int idx = 0;
                for (int k = i - size / 2; k <= i + size / 2; k++)
                {
                    for (int l = j - size / 2; l <= j + size / 2; l++)
                    {
                        pTmp[idx++] = bmp.at<Vec3b>(k, l)[channel];
                    }
                }
                //sorting
                for (int k = 0; k < size * size - 1; k++)
                {
                    int idx = k, tdata;
                    for (int l = k + 1; l < size * size; l++)
                    {
                        if (pTmp[l] < pTmp[idx]) idx = l;
                    }
                    if (idx != k)
                    {
                        tdata = pTmp[idx];
                        pTmp[idx] = pTmp[k];
                        pTmp[k] = tdata;
                    }
                }
                //assignment
                bmp_filted.at<Vec3b>(i,j)[channel] = pTmp[(size * size) / 2];
            }
        }
    }

    namedWindow("display",WINDOW_NORMAL);
    imshow("display", bmp_filted);
    waitKey(0);
    return bmp_filted;
}

////移动一次，像素会平移六分之一的图像的长度
//Mat Translation(Mat bmp,int n=0) {
//    enum n {
//        UP, DOWN, LEFT, RIGHT
//    };
//    int mr = (int)bmp.rows / 6;
//    int mc = (int)bmp.cols / 6;
//    Mat bmp_filted;
//    Mat bmp_tran;
//    switch(bmp.type()) {
//        case 0://灰度图
//            bmp_filted = Mat::zeros(2*mr+bmp.rows, 2*mc+bmp.cols, CV_8UC1);
//            for (int i = mr; i < bmp.rows+mr; i++) {
//                for (int j = mc; j < bmp.cols+mc; j++) {
//                    bmp_filted.at<uchar>(i, j) = bmp.at<uchar>(i, j);
//                }
//            }
//            break;
//        case 16://彩色图
//            bmp_filted = Mat::zeros(bmp.rows + mr, mc + bmp.cols, CV_8UC3);
//            for (int i = 0; i < bmp.rows; i++) {
//                for (int j = 0; j < bmp.cols; j++) {
//                    for(int channel = 0;channel<3;channel++){
//                        bmp_filted.at<uchar>(i, j) = bmp.at<uchar>(i, j);
//                    }
//                }
//            }
//            break;
//        default://其他
//            break;
//    }
//}
//
//Mat DIP::Translation()
//{
//
//}

//根据模板滤波
uchar filter1(Mat bmp,const double filter[],int m,int n) {
    int sum = 0;
    for (int i = m - 1; i < m + 2; i++) {
        for (int j = n - 1; j < n + 2; j++) {
            sum += filter[3*(i - m + 1)+j-n+1] * bmp.at<uchar>(i, j);
        }
    }
    if (sum < 0) {
        return 0;
    }
    else {
        if (sum > 255) {
            return 255;
        }
        else {
            return sum;
        }
    }
   
}
Mat DIP::Low_pass_filtering()const
{
    //低通模板
    const double filter[9] = { 1/9 };
    Mat low = bmp.clone();
    for (int i = 1; i < bmp.rows - 1; i++) {
        for (int j = 1; j < bmp.cols - 1; j++) {
            low.at<uchar>(i, j) = filter1(bmp, filter, i, j);
        }
    }
    return low;
}


Mat DIP::binary()
{
    Mat bi = Mat::zeros(bmp.rows, bmp.cols, CV_8UC1);
    //求全局平均值
    double sum = 0;
    for (int i = 0; i < bmp.rows; i++) {
        for (int j = 0; j < bmp.cols; j++) {
            sum +=bmp.at<uchar>(i, j);
        }
    }
    sum /=  bmp.rows * bmp.cols;
    uchar average = (uchar)sum;
    //大于平均值为255，否则为0
    for (int i = 0; i < bmp.rows; i++) {
        for (int j = 0; j < bmp.cols; j++) {
            if (bmp.at<uchar>(i, j) > average) {
                bi.at<uchar>(i, j) = 255;
            }
        }
    }
    return bi;
}

Mat DIP::High_pass_filtering()const
{
    //高通模板
    const double filter[9] = { -1,-1,-1,
                            -1, 8,-1,
                            -1,-1,-1 };
    Mat high = bmp.clone();
    for (int i = 1; i < bmp.rows - 1; i++) {
        for (int j = 1; j < bmp.cols - 1; j++) {
            high.at<uchar>(i, j) = filter1(bmp,filter,i,j);
        }
    }
    return high;
}
