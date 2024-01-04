#include "DIP.h"

DIP::DIP()
{
    b_color = 0;
}

void DIP::pre_process_color(string ImageName)
{
    //landsat的bgr分别是234波段，全色是第8波段
    Mat mss = imread(ImageName, IMREAD_UNCHANGED);   // 读入图片 

    if (mss.empty())     // 判断文件是否正常打开  
    {
        cout << "Can not load image " << ImageName << "\n";
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

    namedWindow("mss", WINDOW_NORMAL);  //若图片太大，用WINDOW_NORMAL的方式可以让图片的显示大小随窗口大小缩放
    imshow("mss", bmp);  // 显示图片 
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

DIP::DIP(string ImageName,bool b_color1)
{
    if(!b_color1){
        b_color = 0;
        bmp = imread(ImageName, IMREAD_GRAYSCALE);
        cout << ImageName << ":" << endl;
        cout << "depth = " << bmp.depth() << endl << "channels = " << bmp.channels() << endl;
        cout << "Number of rows = " << bmp.rows << endl << "Number of columns = " << bmp.cols << endl;
        cout << "Dimension = " << bmp.dims << endl << "Number of bytes per element = " << bmp.elemSize() << endl;
        cout << "Number of bytes per channel per element = " << bmp.elemSize1() << endl << "type = " << bmp.type() << endl;
    }
    else {
        b_color = 1;
        bmp = imread(ImageName, IMREAD_COLOR);
        cout << ImageName << ":" << endl;
        cout << "depth = " << bmp.depth() << endl << "channels = " << bmp.channels() << endl;
        cout << "Number of rows = " << bmp.rows << endl << "Number of columns = " << bmp.cols << endl;
        cout << "Dimension = " << bmp.dims << endl << "Number of bytes per element = " << bmp.elemSize() << endl;
        cout << "Number of bytes per channel per element = " << bmp.elemSize1() << endl << "type = " << bmp.type() << endl;
    }
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
            //assignment
            pBmp_filted[i * bmp_filted.cols + j] = pTmp[(size * size) / 2];
        }
    }

    namedWindow("Median_Filtering");
    imshow("Median_Filtering", bmp_filted);
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

Mat DIP::Linear_stretching_gray() const
{
    //Frequency_distribution vgray(bmp, 0);
    int vgray[256] = { 0 };//灰度频率直方图
    int value = 20;//线性拉伸的程度
    int numa = (int)bmp.rows * bmp.cols * value / 100;//初始的像素频率
    int numb = (int)bmp.rows * bmp.cols * (100 - value) / 100;//最终的像素频率
    int c = 0, d = 255;//目标
    int a= 0, b = 255 ; // 左值数组和右值数组
    for (int i = 0; i < bmp.rows; i++) {
        for (int j = 0; j < bmp.cols; j++) {
            vgray[bmp.at<uchar>(i, j)] += 1;
        }
    }
    //计算左值
        for (int i = 0, sum = 0 ; i < 256; i++) {
            sum+= vgray[i];
            if (sum > numa) {
                a = i;
                break;
            }
        }
    //计算右值
        for (int i = 255, sum =  0 ; i >= 0; i--) {
            sum += vgray[i];
            if (sum > numa) {
                b = i;
                break;
            }
        }
    //拉伸,新图为bmpnew
    Mat bmpnew = bmp.clone();
    for (int i = 0; i < bmp.rows; i++) {
        for (int j = 0; j < bmp.cols; j++) {
                //处理函数
                bmpnew.at<uchar>(i, j) = Linear_stretching1(bmp.at<uchar>(i, j), a, b);
        }
    }
    namedWindow("Linear_stretching", WINDOW_NORMAL);  //若图片太大，用WINDOW_NORMAL的方式可以让图片的显示大小随窗口大小缩放
    imshow("Linear_stretching", bmpnew);// 显示图片 
    waitKey();

    return bmpnew;
}

Mat DIP::Median_Filtering_color()const
{
    Mat bmp_filted = bmp.clone();
    int size = 3;//窗口大小
    uchar* pTmp = new uchar[size * size];
    for(int channel=0;channel<bmp.channels();channel++){
        for (int i = size / 2; i < bmp_filted.rows - size / 2; i++)
        {
            for (int j = size / 2; j < bmp_filted.cols - size / 2; j++)
            {
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

    namedWindow("Median_Filtering_color",WINDOW_NORMAL);
    imshow("Median_Filtering_color", bmp_filted);
    waitKey(0);
    return bmp_filted;
}
    

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
    const double filter[9] = { 1.0/9.0 };
    Mat low = bmp.clone();
    for (int i = 1; i < bmp.rows - 1; i++) {
        for (int j = 1; j < bmp.cols - 1; j++) {
            low.at<uchar>(i, j) = filter1(bmp, filter, i, j);
        }
    }
    namedWindow("Low_pass_filtering");
    imshow("Low_pass_filtering", low);
    waitKey(0);
    return low;
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
    namedWindow("High_pass_filtering");
    imshow("High_pass_filtering", high);
    waitKey(0);
    return high;
}

Mat DIP::binary(uchar T)
{
    Mat bi = Mat::zeros(bmp.rows, bmp.cols, CV_8UC1);
    for (int i = 0; i < bmp.rows; i++) {
        for (int j = 0; j < bmp.cols; j++) {
            if (bmp.at<uchar>(i, j) >= T) {
                bi.at<uchar>(i, j) = 255;
            }
        }
    }
    namedWindow("binary");
    imshow("binary", bi);
    waitKey(0);
    return bi;
}

Mat DIP::binary2(uchar T)
{
    Frequency_distribution bmp_dis(bmp,0);
    Mat bi = Mat::zeros(bmp.rows, bmp.cols, CV_8UC1);
    for (int i = 0; i < bmp.rows; i++) {
        for (int j = 0; j < bmp.cols; j++) {
            if (bmp.at<uchar>(i, j) >= T) {
                bi.at<uchar>(i, j) = 255;
            }
        }
    }
    cout << "The variance ratio is:" <<bmp_dis.var_ratio(T)<< endl;
    namedWindow("binary");
    imshow("binary", bi);
    waitKey(0);
    return bi;
}

Mat DIP::Getbmp()
{
    Mat a = bmp.clone();
    return a;
}


void test()
{
    DIP mss, pan;
    //mss.pre_process_color("D:\\testpic\\mss.tif");
    pan.pre_process_pan("D:\\testpic\\pan.tif");
    Mat a = pan.Getbmp();
    Frequency_distribution pan_fre(a, 0);
    //mss.Linear_stretching();
    pan.binary2(45);
    /*DIP color("D:\\testpic\\zhupi.jpg",1);
    DIP gray("D:\\testpic\\pic_gray.bmp");
    gray.Median_Filtering();
    gray.High_pass_filtering();
    gray.Low_pass_filtering();
    gray.binary2(140);
    gray.Linear_stretching_gray();
    color.Linear_stretching();
    color.Median_Filtering_color();*/
}
