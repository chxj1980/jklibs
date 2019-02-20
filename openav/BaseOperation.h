#pragma once

#ifndef __BASE_OPERATION_HEADER
#define __BASE_OPERATION_HEADER

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"

#if CV_MAJOR_VERSION == 4
#define CV_VERSION_4
#else
#define CV_VERSION_3
#endif

#define LOG(fmt, ...)  \
	do {   \
	printf("[%s:%d] => ", __FILE__, __LINE__); \
	printf(fmt, ##__VA_ARGS__); \
	} while (0);

#ifdef _WIN32
#ifdef _DEBUG
#pragma comment(lib, "opencv_core330d.lib")
#pragma comment(lib, "opencv_highgui330d.lib")
#pragma comment(lib, "opencv_videoio330d.lib")
#pragma comment(lib, "opencv_imgproc330d.lib")
#pragma comment(lib, "opencv_imgcodecs330d.lib")
#pragma comment(lib, "opencv_objdetect330d.lib")
#else
#pragma comment(lib, "opencv_core320d.lib")
#pragma comment(lib, "opencv_highgui320d.lib")
#pragma comment(lib, "opencv_videoio320d.lib")
#pragma comment(lib, "opencv_imgproc320d.lib")
#pragma comment(lib, "opencv_imgcodecs320d.lib")
#pragma comment(lib, "opencv_objdetect320d.lib")
#endif
#endif

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

class BaseOperation
{
public:
	BaseOperation();
	~BaseOperation();

protected:
	void DrawEllipse(cv::Mat img, double angle);
	void DrawFilledCircle(cv::Mat img, cv::Point center);

	void DrawPolygon(cv::Mat img);
	void DrawLine(cv::Mat img, cv::Point start, cv::Point end);

	void ColorReduce(cv::Mat &inputImage, cv::Mat &outputImage, int div);

private:

};

#endif
