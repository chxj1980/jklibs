//
// Created by jmdvirus on 2017/6/8.
//

#include"opencv2/highgui.hpp"
#include"stdio.h"

int main(int argc, char** argv)
{
    cvNamedWindow("video");
    CvCapture* capture = 0;
	cv::VideoCapture vcap(0);

    // 首先是要通过摄像设备来得到一个CvCapture对象
    if(1 == argc)
    {   // 从摄像头获取初始化对象CvCapture
		capture = cvCaptureFromCAM(0);
        //capture = cvCreateCameraCapture(0);
    }
    else
    {   // 从视频文件中获取初始化对象CvCapture
        capture = cvCreateCameraCapture(atoi(argv[1]));
    }
    // 如果没有获取到有效的CvCapture对象，则返回 -1 终止程序运行
    if(!capture)
    {
        return -1;
    }

    IplImage* frame;
    //指定视频中每一帧的大小（我的摄像头拍摄下的图片均是160*120的）
    CvSize size = cvSize(640,480);
    //需要初始化一个写视频文件的对象，这里注意使用的编解码器格式是MJPG  帧率设置为5
    CvVideoWriter* videoWriter =
            cvCreateVideoWriter("test.avi",CV_FOURCC('M','J','P','G'),5,size);

	cv::Mat pframe;
    char keyCode;
    //每隔30ms，从摄像头中取出一帧
    while(1)
    {
		//得到从摄像头中获取的帧
		vcap >> pframe;
		//frame = cvQueryFrame(capture);
		//if (!frame) break;
        //将帧写入视频文件中
        //cvWriteFrame(videoWriter,frame);
        //cvShowImage("video",pframe);
		cv::imshow("video", pframe);
		keyCode = cvWaitKey(30);
		if (keyCode == 27)
		{
			break;
		}
    }

    cvReleaseVideoWriter(&videoWriter);
    cvReleaseImage(&frame);
    cvDestroyWindow("video");

    return 0;
}