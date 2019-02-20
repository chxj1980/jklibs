//
// Created by jmdvirus on 2017/6/8.
//


#include "CVFaceDetect.h"

#ifdef DEBUG_DURATION
#ifndef _WIN32
#include <sys/time.h>
#else
#include <Windows.h>
#endif
#endif

#ifndef _WIN32
#define LOG_DEBUG(fmt, args...) \
    do {    \
        printf("Debug: "); \
        printf(fmt, ##args); \
    } while(0)
#define LOG_ERROR(fmt, args...) \
    do {    \
        printf("Error: "); \
        printf(fmt, ##args); \
    } while(0)
#else
#define LOG_DEBUG printf
#define LOG_ERROR printf
#endif

#ifdef _WIN32
#include <Windows.h>
#define OPENCV_FONTFACE_PATH "D:\\data\\work\\source\\library\\opencv-3.2.0\\data\\haarcascades\\"
#elif __DARWIN
#define OPENCV_FONTFACE_PATH "/Users/jmdvirus/jmd/proj/app/source/opencv-3.2.0/data/haarcascades/"
#else
#define OPENCV_FONTFACE_PATH "/svn/public/lib/opencv-3.2.0/x64/share/OpenCV/haarcascades/"
//#define OPENCV_FONTFACE_PATH "/opt/Arges/conf/hogcascades/"
#endif
#define OPENCV_FONTFACE_FILE "haarcascade_frontalface_alt.xml"
//#define OPENCV_FONTFACE_FILE "haarcascade_frontalface_alt2.xml"

#ifdef DEBUG_DURATION
time_t get_time_ms() {
#ifndef _WIN32
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec*1000 + tv.tv_usec/1000;
#else
	SYSTEMTIME st;
	GetLocalTime(&st);
	return st.wSecond*1000 + st.wMilliseconds;
#endif
}
#endif

/*---- detectFaces函数 -------
检测灰度图片中的人脸，返回人脸矩形坐标(x,y,width,height)
因为可能检测出多个人脸，所以返回类型为vetor<Rect>
------------------------------------------------------*/
std::vector<cv::Rect> CVFaceDetect::detectFaces(cv::UMat img_gray){
    cv::CascadeClassifier faces_cascade;
    int ret = faces_cascade.load(OPENCV_FONTFACE_PATH OPENCV_FONTFACE_FILE);

    detect_face_rects_.clear();
    if (!ret) {
        LOG_ERROR("local frontface file failed [ %s ]\n", OPENCV_FONTFACE_PATH OPENCV_FONTFACE_FILE);
        return detect_face_rects_;
    }
#ifdef DEBUG_DURATION
    time_t start = get_time_ms();
#endif
    faces_cascade.detectMultiScale(img_gray,detect_face_rects_,1.2, 2,
		0|cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30) );
#ifdef DEBUG_DURATION
    time_t end = get_time_ms();
    LOG_DEBUG("detect multi scale time %ld ms (%ld, %ld)\n", end - start, end, start);
#endif
    return detect_face_rects_;
}

std::vector<cv::Rect> CVFaceDetect::detectFaces2(cv::Mat img_gray) {
	cv::CascadeClassifier faces_cascade;
	int ret = faces_cascade.load(OPENCV_FONTFACE_PATH OPENCV_FONTFACE_FILE);

	detect_face_rects_.clear();
	if (!ret) {
		LOG_ERROR("local frontface file failed [ %s ]\n", OPENCV_FONTFACE_PATH OPENCV_FONTFACE_FILE);
		return detect_face_rects_;
	}
#ifdef DEBUG_DURATION
	time_t start = get_time_ms();
#endif
	faces_cascade.detectMultiScale(img_gray, detect_face_rects_, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));
#ifdef DEBUG_DURATION
	time_t end = get_time_ms();
	LOG_DEBUG("detect multi scale time %ld ms (%ld, %ld)\n", end - start, end, start);
#endif
	return detect_face_rects_;
}

std::vector<cv::Rect> CVFaceDetect::detect_face(const char *buffer, int len, cv::Size size) {

    cv::Mat l_image(size.height + size.height/2, size.width, CV_8UC1, (char*)buffer);
    cv::UMat ori_img;
    l_image.copyTo(ori_img);
    cv::UMat l_img_gray;
    cv::cvtColor(ori_img, l_img_gray, cv::COLOR_YUV420p2GRAY);
    cv::equalizeHist(l_img_gray, l_img_gray);
    return detectFaces(l_img_gray);
//    return detectFaces2(l_img_gray);
}

std::vector<cv::Rect> CVFaceDetect::detect_face_h264(cv::UMat frame) {

	cv::UMat l_img_gray;
	cv::cvtColor(frame, l_img_gray, cv::COLOR_RGB2GRAY);
	cv::equalizeHist(l_img_gray, l_img_gray);
	return detectFaces(l_img_gray);
}

std::vector<cv::Rect> CVFaceDetect::detect_face_image(const char *filename) {
    //IplImage* image = cvLoadImage( filename, 1 );

    //cv::Mat l_image = cv::cvarrToMat(image, false);
	cv::Mat l_image = cv::imread(filename);
    cv::UMat l_img_gray;
    cv::cvtColor(l_image, l_img_gray, cv::COLOR_BGR2GRAY);
    cv::equalizeHist(l_img_gray, l_img_gray);
    vector<cv::Rect> faces = detectFaces(l_img_gray);

    //cvReleaseImage(&image);
    return faces;
}

int CVFaceDetect::face_detect_draw_image(const char *filename) {
#ifndef __NO_HIGHGUI
    cv::namedWindow("face detect");
#endif

    //IplImage* image = cvLoadImage( filename, 1);
	cv::Mat image = cv::imread(filename);

    static cv::Scalar colors(100, 100, 100);
    std::vector<cv::Rect> ret = detect_face_image(filename);
    for (int i = 0; i < ret.size(); i++) {
        cv::Rect dr = ret[i];
		cv::Rect cr(dr.x, dr.y, dr.width, dr.height);
        LOG_DEBUG("Out index [%d] [%d, %d, %d, %d]\n", i, dr.x, dr.y, dr.width, dr.height);
		cv::rectangle(image, cr, colors);
    }
#ifndef __NO_HIGHGUI
	cv::imshow("face detect", image);
    cv::waitKey(0);
#endif
    //cvReleaseImage(&image);
#ifndef __NO_HIGHGUI
	cv::destroyWindow("face detect");
#endif

    return 0;
}

int CVFaceDetect::face_detect_draw_video(const char *filename, cv::Size size) {

    int frame_size = (size.height * size.width) * 3 /2;
    char *frame = new char[frame_size];
	FILE * file = NULL;
#ifdef _WIN32
	fopen_s(&file, filename, "rb");
#else
    file = fopen(filename, "r");
#endif
    if (!file) return -1;

    static cv::Scalar colors(100, 100, 100);
#ifndef __NO_HIGHGUI
    cv::namedWindow("video");
#endif

    while (true) {
        int n = fread(frame, 1, frame_size, file);
        if (n <= 0) {
            LOG_DEBUG("read data failed ret %d\n", n);
            break;
        }
        cv::Mat l_image(size.height + size.height/2, size.width, CV_8UC1, (char*)frame);
		cv::Mat do_image = l_image.clone();
		do_image.data = l_image.data;
        //IplImage *do_image = cvCreateImage(size, IPL_DEPTH_8U, 1);
        //do_image->imageData = (char *) l_image.data;
        std::vector<cv::Rect> ret = detect_face(frame, frame_size, size);
        for (int i = 0; i < ret.size(); i++) {
            cv::Rect dr = ret[i];
			cv::Rect cr(dr.x, dr.y, dr.width, dr.height);
            LOG_DEBUG("Out index [%d] [%d, %d, %d, %d]\n", i, dr.x, dr.y, dr.width, dr.height);
			cv::rectangle(do_image, cr, colors);
        }
#ifndef __NO_HIGHGUI
        cv::imshow("video", do_image);
		cv::waitKey(10);
#endif
		//cv::ReleaseImage(&do_image);
    }
#ifndef __NO_HIGHGUI
	cv::destroyWindow("video");
#endif
    delete []frame;
    fclose(file);

    return 0;
}

int CVFaceDetect::face_detect_draw_video_h264(const char *filename, cv::Size size) {

	int frame_size = 1024 * 1024 * 8;
	char *frame = new char[frame_size];
	FILE * file = NULL;
#ifdef _WIN32
	fopen_s(&file, filename, "rb");
#else
    file = fopen(filename, "rb");
#endif
	if (!file) return -1;

	static cv::Scalar colors(100, 100, 100);
#ifndef __NO_HIGHGUI
	cv::namedWindow("video");
#endif

	while (true) {
		int n = fread(frame, 1, frame_size, file);
		if (n <= 0) {
			LOG_DEBUG("read data failed ret %d\n", n);
			break;
		}
		cv::Mat l_image(size.height + size.height / 2, size.width, CV_8UC1, (char*)frame);
		cv::Mat do_image = l_image.clone();
		do_image.data = l_image.data;
		//IplImage *do_image = cvCreateImage(size, IPL_DEPTH_8U, 1);
		//do_image->imageData = (char *)l_image.data;
		std::vector<cv::Rect> ret = detect_face(frame, frame_size, size);
		for (int i = 0; i < ret.size(); i++) {
			cv::Rect dr = ret[i];
			cv::Rect cr(dr.x, dr.y, dr.width, dr.height);
			LOG_DEBUG("Out index [%d] [%d, %d, %d, %d]\n", i, dr.x, dr.y, dr.width, dr.height);
			cv::rectangle(do_image, cr, colors);
			//cvRectangleR(do_image, cr, colors);
		}
#ifndef __NO_HIGHGUI
		cv::imshow("video", do_image);
		cv::waitKey(10);
#endif
		//cv::releaseImage(&do_image);
	}
#ifndef __NO_HIGHGUI
	cv::destroyWindow("video");
#endif
	delete[]frame;
	fclose(file);

	return 0;
}

