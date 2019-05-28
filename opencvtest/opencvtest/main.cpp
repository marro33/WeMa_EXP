//
//  main.cpp
//  opencvtest
//
//  Created by Gaojian on 2019/3/11.
//  Copyright © 2019年 Gaojian. All rights reserved.
//
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "lbpcascades/funset.hpp"
#include "bayes/BayesClassifier.hpp"
using namespace cv;
using namespace std;

void showImage();
void getSeconds(unsigned long *par);
void coutTest();
void test_face_detect();
void QRDetection();


int main()
{
//    bayesDemo();
//    test_face_detect();

    QRDetection();
    return 0;
}

void QRDetection(){
//    Mat  = imread( "pic\\456.jpg", 1 );
    Mat src = imread("/Users/Grenouille/Code/MyXcodeProject/opencvtest/testdata/detection/2.jpg", 1);
    if(src.empty())
    {
        fprintf(stderr, "Can not load image！\n");
    }

    Mat src_all=src.clone();

    //彩色图转灰度图
    Mat src_gray;
    cvtColor( src, src_gray, COLOR_BGR2GRAY);

    //对图像进行平滑处理
    blur( src_gray, src_gray, Size(3,3) );

    //使灰度图象直方图均衡化
    equalizeHist( src_gray, src_gray );

    namedWindow("src_gray");
    imshow("src_gray",src_gray);         //灰度图

    //指定112阀值进行二值化
    Mat threshold_output;
    threshold( src_gray, threshold_output, 112, 255, THRESH_BINARY );

    namedWindow("二值化后输出");
    imshow("二值化后输出",threshold_output);
    waitKey(1000);
    

}


void test_face_detect(){
    int ret = test_face_detect_LBP();
    if (ret == 0) fprintf(stdout, "========== test success ==========\n");
    else fprintf(stderr, "########## test fail ##########\n");

}


void getSeconds(unsigned long *par){
    int a[] = {0,1,2,3};
    int * b = begin(a);
    int * e = end(a);
    for (; b != e; ++b) {
        cout<<*b<<endl;
    }
    *par = time(NULL);
    return;
}


void showImage(String path){
    Mat image;
    image = imread("/Users/Grenouille/Code/MyXcodeProject/opencvtest/testdata/detectionz1.jpg");
    if ( !image.data )
    {
        printf("No image data \n");

    }
    namedWindow("Display Image", WINDOW_AUTOSIZE);
    imshow("Display Image", image);
    waitKey(1000);
    destroyWindow("Display Image");
}


void adaBoostTest(){
    const int kWidth = 512, kHeight = 512;
    Vec3d red (0, 0, 255), green(0,255,0),blue (255,0,0);
    Mat image = Mat:: zeros(kHeight, kWidth, CV_8UC3);
}
