//
//  BayesClassifier.cpp
//  opencvtest
//
//  Created by Gaojian on 2019/3/28.
//  Copyright © 2019年 Gaojian. All rights reserved.
//贝叶斯分类器主要用于解决正态情况下的分类问题

#include "BayesClassifier.hpp"
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace cv::ml;
using namespace std;
//
void bayesDemo(){
    const int kWight = 512;
    const int kHeight = 512;

    Vec3b red(0,0,255), green(0,255,0), blue(255,0,0),white(0,0,0);

    Mat image = Mat::zeros(kHeight, kWight, CV_8UC3);//显示结果的图像

    int lables[30];

    for(int i=0; i < 10; i++){
        lables[i] = 1;
        lables[i+10] = 2;
        lables[i+20] = 3;
    }//初始化标签


    Mat trainResponse(30,1,CV_32SC1, lables);//生成训练样本特征向量数组

    float trainingData[30][2];

    RNG rng;//用于生成随机数

    for(int i = 0; i < 10; i++){
        trainingData[i][0] = 250 + static_cast<float>(rng.gaussian(30));
        trainingData[i][1] = 250 + static_cast<float>(rng.gaussian(30));

        trainingData[i+10][0] = 150  + static_cast<float>(rng.gaussian(30));
        trainingData[i+10][1] = 150 + static_cast<float>(rng.gaussian(30));


        trainingData[i+20][0] = 320 + static_cast<float>(rng.gaussian(30));
        trainingData[i+20][1] = 150 + static_cast<float>(rng.gaussian(30));
    }

    Mat trainDataMat(30, 2, CV_32FC1, trainingData);
    cout<<"training data"<<endl;

    //训练贝叶斯分类器
//    CvNormalBayesClassifier bayesClassifier;
    Ptr<NormalBayesClassifier> bayesClassifier = NormalBayesClassifier::create();
    //    bayesClassifier.train(trainData, trainResponse);
//根据制造的训练样本和标注，包装训练数据
    Ptr<TrainData> trainData = TrainData::create(trainDataMat, ROW_SAMPLE, trainResponse);
    bayesClassifier->train(trainData);

    for (int i = 0; i < image.rows; ++i){
        for(int j = 0; j < image.cols; ++j){
            //生成测试样本特征向量
            Mat sampleMat = (Mat_<float>(1,2) << j, i);
            //使用贝叶斯分类器进行预测
            float response = bayesClassifier->predict(sampleMat);

            if(response == 1){
                image.at<Vec3b>(i,j) = red;
            }else if(response == 2){
                image.at<Vec3b>(i,j) = green;
            }else if(response == 3){
                image.at<Vec3b>(i,j) = blue;
            }else{
                image.at<Vec3b>(i,j) = white;
            }
        }
    }

//    显示训练样本
    for(int i = 0; i < 30; i++){
        const float* v = trainDataMat.ptr<float>(i);
        Point pt = Point((int)v[0], (int)v[1]);
        if(lables[i] == 1){
            circle(image, pt, 5, Scalar::all(0), -1 ,8);
        }else if(lables[i] == 2){
            circle(image, pt, 5, Scalar::all(128), -1 ,8);
        }else if(lables[i] == 3){
            circle(image, pt, 5, Scalar::all(255), -1 ,8);
        }
    }

    imshow("Bayes classifier demo", image);
    waitKey();

}
