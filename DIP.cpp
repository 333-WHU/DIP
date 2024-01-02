#include "DIP.h"

Mat pre_process_color(string ImageName)
{
    //landsat的bgr分别是234波段，全色是第8波段
    Mat mss = imread(ImageName, IMREAD_UNCHANGED);   // 读入图片 

    if (mss.empty())     // 判断文件是否正常打开  
    {
        fprintf(stderr, "Can not load image %s\n", ImageName);
        waitKey(6000);  // 等待6000 ms后窗口自动关闭   
    }
    cout << ImageName << ":" << endl;
    cout << "depth = " << mss.depth() << endl << "channels = " << mss.channels() << endl;
    cout << "Number of rows = " << mss.rows << endl << "Number of columns = " << mss.cols << endl;
    cout << "Dimension = " << mss.dims << endl << "Number of bytes per element = " << mss.elemSize() << endl;
    cout << "Number of bytes per channel per element = " << mss.elemSize1() << endl << "type = " << mss.type() << endl;


    Mat b_g_r_ir[4], color_arr[3], color;
    split(mss, b_g_r_ir);

    color_arr[0] = b_g_r_ir[2];
    color_arr[1] = b_g_r_ir[1];
    color_arr[2] = b_g_r_ir[0];
    merge(color_arr, 3, color);

    //各波段数值范围大概在0-1001之间，所以建议转至8比特时，除以4
    color.convertTo(color, CV_8UC3, 1.0 / 4.0, 0);

    namedWindow("color", WINDOW_NORMAL);  //若图片太大，用WINDOW_NORMAL的方式可以让图片的显示大小随窗口大小缩放
    imshow("color", color);  // 显示图片 
    waitKey();

    //若仅需要裁剪部分做后续处理
    //Mat subset = color(Range(2650, 3150), Range(5400, 5900)); //subset共享color的部分数据，前面为行范围，后面为列范围
    //imwrite("mss_subset.bmp", subset); //16U的数据可以保存至png、jpeg2000或tiff文件，其中png和jpeg2000需要设置压缩参数前面为行范围，后面为列范围

    return color;

}

Mat pre_process_pan(string ImageName)
{
    Mat pan = imread(ImageName, IMREAD_UNCHANGED);    //读入全色波段
    cout << ImageName << ":" << endl;
    cout << "depth = " << pan.depth() << endl << "channels = " << pan.channels() << endl;
    cout << "Number of rows = " << pan.rows << endl << "Number of columns = " << pan.cols << endl;
    cout << "Dimension = " << pan.dims << endl << "Number of bytes per element = " << pan.elemSize() << endl;
    cout << "Number of bytes per channel per element = " << pan.elemSize1() << endl << "type = " << pan.type() << endl;

    //最大值552， 除以4
    pan.convertTo(pan, CV_8UC1, 1.0 / 4.0, 0);

    namedWindow("pan", WINDOW_NORMAL);  //若图片太大，用WINDOW_NORMAL的方式可以让图片的显示大小随窗口大小缩放
    imshow("pan", pan);  // 显示图片 
    waitKey();

    return pan;

}

Mat Median_Filtering(string ImageName) {
    Mat bmp = imread(ImageName,IMREAD_GRAYSCALE);
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

Mat Linear_stretching(string ImageName)
{
    Mat bmp = imread(ImageName, IMREAD_COLOR);
    int vgray[256][3];//灰度频率
    int value = 2;//线性拉伸的程度
    int numa = (int)bmp.rows * bmp.cols * value / 100;//初始的像素频率
    int numb = (int)bmp.rows * bmp.cols * (100 - value) / 100;//最终的像素频率
    int c = 0, d = 255;//目标
    int a[3] = { 0 }, b[3] = { 255 };
    MatIterator_<Vec3b> it;
    for (it = bmp.begin<Vec3b>(); it != bmp.end<Vec3b>(); it++) {

    }//迭代器遍历
    for (int i = 0; i < bmp.rows; i++) {
        for (int j = 0; j < bmp.cols; j++) {
            for (int channel = 0; channel < bmp.channels(); channel++) {
                vgray[bmp.at[channe]]
            }
        }
    }
}
