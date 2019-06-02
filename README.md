
# 毕业实验项目

WeMa 项目主要功能：
识别隐形点阵二维码

二维码图片：




## 相关项目，框架说明

* WeMa： 扫描隐形二维码主要功能的实现框架。

* swift with opencv: 一个能在手机端运行opencv框架的app。

* OpenCvTest： 工程在mac运行c++语言，导入openCv库，用于基于库的简单图像处理和实验。

* IPDFC： 基于iphone摄像头的图像捕捉框架，用于素材的收集，以及初期简单实验环境的搭建。

* MADRectDetect: 类似IPDFC， 增加可以手动调节选框大小的功能。

* ICGVideoTrimer： 视频流剪辑软件

## 项目思路

1. 使用IPDFC&MAD对iphone摄像头的最小识别框大小进行测定。
2.

## 关键代码说明
```
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
