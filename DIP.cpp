#include "DIP.h"

void DIP::pre_process_color(string ImageName)
{
    //landsat��bgr�ֱ���234���Σ�ȫɫ�ǵ�8����
    Mat mss = imread(ImageName, IMREAD_UNCHANGED);   // ����ͼƬ 

    if (mss.empty())     // �ж��ļ��Ƿ�������  
    {
        cout<< "Can not load image "<<ImageName<<"\n";
        waitKey(6000);  // �ȴ�6000 ms�󴰿��Զ��ر�   
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

    //��������ֵ��Χ�����0-1001֮�䣬���Խ���ת��8����ʱ������4
    bmp.convertTo(bmp, CV_8UC3, 1.0 / 4.0, 0);

    namedWindow("color", WINDOW_NORMAL);  //��ͼƬ̫����WINDOW_NORMAL�ķ�ʽ������ͼƬ����ʾ��С�洰�ڴ�С����
    imshow("color", bmp);  // ��ʾͼƬ 
    waitKey();

    //������Ҫ�ü���������������
    //Mat subset = color(Range(2650, 3150), Range(5400, 5900)); //subset����color�Ĳ������ݣ�ǰ��Ϊ�з�Χ������Ϊ�з�Χ
    //imwrite("mss_subset.bmp", subset); //16U�����ݿ��Ա�����png��jpeg2000��tiff�ļ�������png��jpeg2000��Ҫ����ѹ������ǰ��Ϊ�з�Χ������Ϊ�з�Χ
}

void DIP::pre_process_pan(string ImageName)
{
    bmp = imread(ImageName, IMREAD_UNCHANGED);    //����ȫɫ����
    cout << ImageName << ":" << endl;
    cout << "depth = " << bmp.depth() << endl << "channels = " << bmp.channels() << endl;
    cout << "Number of rows = " << bmp.rows << endl << "Number of columns = " << bmp.cols << endl;
    cout << "Dimension = " << bmp.dims << endl << "Number of bytes per element = " << bmp.elemSize() << endl;
    cout << "Number of bytes per channel per element = " << bmp.elemSize1() << endl << "type = " << bmp.type() << endl;

    //���ֵ552�� ����4
    bmp.convertTo(bmp, CV_8UC1, 1.0 / 4.0, 0);

    namedWindow("pan", WINDOW_NORMAL);  //��ͼƬ̫����WINDOW_NORMAL�ķ�ʽ������ͼƬ����ʾ��С�洰�ڴ�С����
    imshow("pan", bmp);  // ��ʾͼƬ 
    waitKey();

}

Mat DIP::Median_Filtering() const {
    Mat bmp_filted;
    bmp_filted.create(bmp.rows, bmp.cols, CV_8UC1);

    unsigned char* pBmp_filted = bmp_filted.data;
    int size = 3;//���ڴ�С
    uchar* pTmp = new uchar[size * size];//������
    for (int i = size / 2; i < bmp_filted.rows - size / 2; i++)//�ӵ�size/2��ʼ��������size/2��
    {
        for (int j = size / 2; j < bmp_filted.cols - size / 2; j++)//�ӵ�size/2��ʼ��������size/2��
        {   //��������ȴ�ֵ������ķ���
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
            ////�������ջ��Ϊ����ķ���
            //pTmp[0] = *bmp.ptr<uchar>(i - size / 2, j - size / 2);//ջ����󳤶�Ϊsize*size����һ��Ԫ��ֱ����ջ���Դ�С������ջ
            //int length = 1;//��ǰջ�ĳ���
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
            //            }//���С�ڵ�ǰԪ�أ����Ǵ���֮ǰ��Ԫ�أ���ǰԪ��֮�󣨰�����ǰ����Ԫ�س�ջ������һ����
            //        }
            //        if (t == length) {
            //            pTmp[length] = *bmp.ptr<uchar>(k, l);
            //        }//������Ԫ�ش���ջ
            //        length++;//��ջ����ջ�ĳ��ȼ�һ
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

//����Ĵ�����,��t��a��b��Χ������
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
    namedWindow("color", WINDOW_NORMAL);  //��ͼƬ̫����WINDOW_NORMAL�ķ�ʽ������ͼƬ����ʾ��С�洰�ڴ�С����
    imshow("color", bmp);  // ��ʾͼƬ 
    waitKey();
    int vgray[256][3] = { 0 };//�Ҷ�Ƶ��ֱ��ͼ
    int value = 2;//��������ĳ̶�
    int numa = (int)bmp.rows * bmp.cols * value / 100;//��ʼ������Ƶ��
    int numb = (int)bmp.rows * bmp.cols * (100 - value) / 100;//���յ�����Ƶ��
    int c = 0, d = 255;//Ŀ��
    int a[3] = { 0 }, b[3] = { 255 }; // ��ֵ�������ֵ����
    for (int i = 0; i < bmp.rows; i++) {
        for (int j = 0; j < bmp.cols; j++) {
           for (int channel = 0; channel < bmp.channels(); channel++) {
             vgray[bmp.at<Vec3b>(i,j)[channel]][channel]+=1;
            }
        }
    }
    //������ֵ
     for(int channel=0;channel<bmp.channels();channel++){
        for (int i = 0,sum[3] = { 0 }; i < 256; i++){
            sum[channel] += vgray[i][channel];
            if (sum[channel] > numa) {
                a[channel] = i;
                break;
            }
        }
     }
     //������ֵ
     for (int channel = 0; channel < bmp.channels(); channel++) {
         for (int i = 255,sum[3] = { 0 }; i >=0; i--) {
             sum[channel] += vgray[i][channel];
             if (sum[channel] > numa) {
                 b[channel] = i;
                 break;
             }
         }
     }
     //����,��ͼΪbmpnew
     Mat bmpnew = bmp.clone();
     for (int i = 0; i < bmp.rows; i++) {
         for (int j = 0; j < bmp.cols; j++) {
             for (int channel = 0; channel < bmp.channels(); channel++) {
                 //������
                 bmpnew.at<Vec3b>(i,j)[channel] = Linear_stretching1(bmp.at<Vec3b>(i, j)[channel], a[channel], b[channel]);
             }
         }
     }
     namedWindow("Linear_stretching", WINDOW_NORMAL);  //��ͼƬ̫����WINDOW_NORMAL�ķ�ʽ������ͼƬ����ʾ��С�洰�ڴ�С����
     imshow("Linear_stretching",bmpnew);// ��ʾͼƬ 
     waitKey();
     
     return bmpnew;
}

Mat DIP::Median_Filtering_color()const
{
    Mat bmp_filted = bmp.clone();
    int size = 3;//���ڴ�С
    uchar* pTmp = new uchar[size * size];//������
    for(int channel=0;channel<bmp.channels();channel++){
        for (int i = size / 2; i < bmp_filted.rows - size / 2; i++)//�ӵ�size/2��ʼ��������size/2��
        {
            for (int j = size / 2; j < bmp_filted.cols - size / 2; j++)//�ӵ�size/2��ʼ��������size/2��
            {   //��������ȴ�ֵ������ķ���
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

////�ƶ�һ�Σ����ػ�ƽ������֮һ��ͼ��ĳ���
//Mat Translation(Mat bmp,int n=0) {
//    enum n {
//        UP, DOWN, LEFT, RIGHT
//    };
//    int mr = (int)bmp.rows / 6;
//    int mc = (int)bmp.cols / 6;
//    Mat bmp_filted;
//    Mat bmp_tran;
//    switch(bmp.type()) {
//        case 0://�Ҷ�ͼ
//            bmp_filted = Mat::zeros(2*mr+bmp.rows, 2*mc+bmp.cols, CV_8UC1);
//            for (int i = mr; i < bmp.rows+mr; i++) {
//                for (int j = mc; j < bmp.cols+mc; j++) {
//                    bmp_filted.at<uchar>(i, j) = bmp.at<uchar>(i, j);
//                }
//            }
//            break;
//        case 16://��ɫͼ
//            bmp_filted = Mat::zeros(bmp.rows + mr, mc + bmp.cols, CV_8UC3);
//            for (int i = 0; i < bmp.rows; i++) {
//                for (int j = 0; j < bmp.cols; j++) {
//                    for(int channel = 0;channel<3;channel++){
//                        bmp_filted.at<uchar>(i, j) = bmp.at<uchar>(i, j);
//                    }
//                }
//            }
//            break;
//        default://����
//            break;
//    }
//}
//
//Mat DIP::Translation()
//{
//
//}

//����ģ���˲�
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
    //��ͨģ��
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
    //��ȫ��ƽ��ֵ
    double sum = 0;
    for (int i = 0; i < bmp.rows; i++) {
        for (int j = 0; j < bmp.cols; j++) {
            sum +=bmp.at<uchar>(i, j);
        }
    }
    sum /=  bmp.rows * bmp.cols;
    uchar average = (uchar)sum;
    //����ƽ��ֵΪ255������Ϊ0
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
    //��ͨģ��
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
