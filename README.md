
# 毕业实验项目

WeMa 项目主要功能：
识别隐形点阵二维码
项目地址：
https://github.com/marro33/WeMa

实验研究说明——开题报告.pptx

https://github.com/marro33/WeMa_EXP/blob/master/%E5%AE%9E%E9%AA%8C%E7%A0%94%E7%A9%B6%E8%AF%B4%E6%98%8E%E2%80%94%E2%80%94%E5%BC%80%E9%A2%98%E6%8A%A5%E5%91%8A.pptx

二维码图片：

![image](https://raw.githubusercontent.com/marro33/WeMa_EXP/master/%E8%BD%AF%E4%BB%B6%E7%95%8C%E9%9D%A2/%E6%89%AB%E6%8F%8F%E6%A0%B7%E4%BE%8B/%E9%9A%90%E5%BD%A2%E7%A0%81.png)

扫描界面
![image][https://github.com/marro33/WeMa/blob/master/%E8%BD%AF%E4%BB%B6%E7%95%8C%E9%9D%A2/%E8%AF%B4%E6%98%8E-%E9%9A%90%E5%BD%A2%E7%A0%81%E9%A1%B5%E9%9D%A2.png?raw=true]


扫描历史结果列表
![image][https://github.com/marro33/WeMa/blob/master/%E8%BD%AF%E4%BB%B6%E7%95%8C%E9%9D%A2/%E8%AF%B4%E6%98%8E-%E5%8E%86%E5%8F%B2%E8%AE%B0%E5%BD%95%E9%A1%B5%E9%9D%A2.png?raw=true]

扫描物流信息
![image][https://github.com/marro33/WeMa/blob/master/%E8%BD%AF%E4%BB%B6%E7%95%8C%E9%9D%A2/%E8%AF%B4%E6%98%8E-%E7%A8%BD%E6%9F%A5%E7%BB%93%E6%9E%9C%E9%A1%B5%E9%9D%A2.png?raw=true]



## 相关项目，框架说明

* WeMa： 扫描隐形二维码主要功能的实现框架。

* swift with opencv: 一个能在手机端运行opencv框架的app。

* OpenCvTest： 工程在mac运行c++语言，导入openCv库，用于基于库的简单图像处理和实验。

* IPDFC： 基于iphone摄像头的图像捕捉框架，用于素材的收集，以及初期简单实验环境的搭建。

* MADRectDetect: 类似IPDFC， 增加可以手动调节选框大小的功能。

* ICGVideoTrimer： 视频流剪辑软件

## 项目思路

1. 使用IPDFC&MAD对iphone摄像头的最小识别框大小进行测定。
2. 基于1中确定的最小识别窗口，对一帧图像进行分割。
3. 对切割后的每一个窗口使用分类器，判别是否满足识别要求。
4. 确定可识别区域，进行识别，校验。

## 关键代码说明
```

//实现接口
QrScanViewController：AVCaptureVideoDataOutputSampleBufferDelegate


var captureSession: AVCaptureSession?
var videoPreviewLayer: AVCaptureVideoPreviewLayer!

//设置输出
let videoDataOutput = AVCaptureVideoDataOutput()
videoDataOutput.videoSettings = [kCVPixelBufferPixelFormatTypeKey as AnyHashable as! String: NSNumber(value: kCVPixelFormatType_420YpCbCr8BiPlanarFullRange)]

//将输出放在一个其他线程，开始session
videoDataOutput.alwaysDiscardsLateVideoFrames = true
let queue = DispatchQueue(label: "vedioDataOutputQueue")
videoDataOutput.setSampleBufferDelegate(self, queue: queue)
captureSession?.addOutput(videoDataOutput)
captureSession?.startRunning()


//隐形码解码接口
//实现抽象方法
func captureOutput(_ output: AVCaptureOutput, didOutput sampleBuffer: CMSampleBuffer, from connection: AVCaptureConnection) {

   //获取 imageBuffer: CVImageBuffer
    let imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer)

    //获取扫描范围块 interestedrect: CGRect
    let interestrect = videoPreviewLayer.metadataOutputRectConverted(fromLayerRect: scanRect)

    // 把 imagerBuffer and interestrect 传给ImageBufferHandler去解码
    if let result = ImageBufferHandler.handleTheBuffer(imageBuffer, interestrect){
        //解码成功，显示结果
        self.process(result)
    }
}
```
在得到CVImageBufferRef之后 使用ImageBufferHandler处理

```
+ (NSString*) handleTheBuffer:(CVImageBufferRef) imageBuffer :(CGRect) interestRect{
    //lock the base address
    //锁定地址后才可访问CVPixelBufferGetBaseAddressOfPlane(imageBuffer, 0);
    CVPixelBufferLockBaseAddress(imageBuffer, 0);

    //size_t 表示对象所能达到的最大长度，常用来接受sizeof（）的返回
    size_t width = CVPixelBufferGetWidth(imageBuffer);
    size_t height = CVPixelBufferGetHeight(imageBuffer);


    // 采用4:2:2 方式
    //yuv中的y所占字节数
    size_t ySize = width * height;
    //yuv中的uv所占的字节数
    size_t uvSize = ySize / 2;

    uint8_t *yuvFrame = (uint8_t*)malloc(uvSize + ySize);
    //获取CVImageBufferRef中的y数据
    uint8_t *yFrame = CVPixelBufferGetBaseAddressOfPlane(imageBuffer, 0);
    memcpy(yuvFrame, yFrame, ySize);
    //获取CMVImageBufferRef中的uv数据
    uint8_t *uvFrame = CVPixelBufferGetBaseAddressOfPlane(imageBuffer, 1);
    memcpy(yuvFrame + ySize, uvFrame, uvSize);


    int yuvSize = (int)ySize + (int)uvSize;
    int tempDataSize = yuvSize / sizeof(int8_t);
    int8_t* tempData = (int8_t*)malloc(yuvSize);

    NSLog(@"tempd: %s + %d",tempData,tempDataSize);
    memcpy(tempData, yuvFrame, yuvSize);

    //开锁
    CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
    //        NSLog(@"%@",@"SS");
    free(yuvFrame);
//    CGRect interestRect = [_layer metadataOutputRectOfInterestForRect:_scanRect];

    int dstLeftT = (int)(interestRect.origin.x*width);
    int dstTopT = (int)(interestRect.origin.y*height);
    int dstWidthT = (int)(interestRect.size.width*width);
    int dstHeightT = (int)(interestRect.size.height*height);

    NSLog(@"interestRect%f,%f", interestRect.size.height, interestRect.size.width);

    NSLog(@"#:%d + %d + %d + %d",dstLeftT,dstTopT,dstWidthT,dstHeightT);


    //在这一步处理图像的时候，需要了解扫描框的具体大小但是明显有错误

//    MyZXPlanarYUVLuminanceSource *source = [[MyZXPlanarYUVLuminanceSource alloc] initWithYuvData:tempData
//                                                        yuvDataLen:tempDataSize dataWidth:(int)width
//                                                        dataHeight:(int)height
//                                                              left:588
//                                                               top:168
//                                                             width:742
//                                                            height:743];
    MyZXPlanarYUVLuminanceSource *source = [[MyZXPlanarYUVLuminanceSource alloc]                                                    initWithYuvData:tempData
                                                                                                                                         yuvDataLen:tempDataSize dataWidth:(int)width
                                                            dataHeight:(int)height
                                                                  left:dstLeftT
                                                                   top:dstTopT
                                                                 width:dstWidthT
                                                                height:dstHeightT];


    ZXBinaryBitmap *bitmap = [ZXBinaryBitmap binaryBitmapWithBinarizer:
                              [[ZXHybridBinarizer alloc] initWithSource:source]];

    NSString *retString = @"";
    int datasize = 10;

    DecodeUtils *decoder = [[DecodeUtils alloc] initWithSide:5 andDataSize:datasize];
    retString = [decoder decodeBitMap:bitmap];

    free(tempData);
    return retString;
}

```
