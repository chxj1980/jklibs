

//#include "stdafx.h"
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "facedetect-dll.h"

#include "Windows.h"

//#pragma comment(lib,"libfacedetect.lib")
#pragma comment(lib,"libfacedetect-x64.lib")
#pragma comment(lib, "opencv_world320d.lib")

//define the buffer size. Do not change the size!
#define DETECT_BUFFER_SIZE 0x20000
using namespace cv;

time_t get_time_ms() {
	SYSTEMTIME st;
	GetLocalTime(&st);
	return st.wSecond * 1000 + st.wMilliseconds;
}

int face_detect_from_video() {
	cv::VideoCapture vcap(0);

	CvScalar colors(10, 10, 200);
	cv::Size size(640, 480);
	cv::Mat frame;

	int doLandmark = true;

	char *pbuffer = (char*)malloc(DETECT_BUFFER_SIZE);
	int *pResults = NULL;
	while (true) {
		vcap >> frame;

		Mat result_frontal = frame.clone();

		cv::Mat gray;
		cv::cvtColor(frame, gray, cv::COLOR_YUV420p2GRAY);

		time_t start_t = get_time_ms();

		pResults = facedetect_frontal((unsigned char*)pbuffer, (unsigned char*)(gray.ptr(0)), gray.cols, gray.rows, (int)gray.step,
			1.2f, 2, 48, 0, doLandmark);

		time_t stop_t = get_time_ms();

		printf("%d faces detected. waste time %d\n", (pResults ? *pResults : 0), stop_t - start_t);

		for (int i = 0; i < (pResults ? *pResults : 0); i++)
		{
			short * p = ((short*)(pResults + 1)) + 142 * i;
			int x = p[0];
			int y = p[1];
			int w = p[2];
			int h = p[3];
			int neighbors = p[4];
			int angle = p[5];

			printf("face_rect=[%d, %d, %d, %d], neighbors=%d, angle=%d\n", x, y, w, h, neighbors, angle);
			rectangle(result_frontal, Rect(x, y, w, h), Scalar(0, 255, 0), 2);
			if (doLandmark)
			{
				for (int j = 0; j < 68; j++)
					circle(result_frontal, Point((int)p[6 + 2 * j], (int)p[6 + 2 * j + 1]), 1, Scalar(0, 255, 0));
			}
		}
		imshow("video", result_frontal);
		waitKey(20);
	}
	while (true) {
		::Sleep(20);
	}
	return 0;
}

int main(int argc, char* argv[])
{

	face_detect_from_video();
	return 0;

	if (argc != 2)
	{
		printf("Usage: %s <video_file_name>\n", argv[0]);
		return -1;
	}
	char *filename = argv[1];
	int width = 1920;
	int height = 1080;

	int len_size = width * height * 3 / 2;
	char *buffer = (char*)malloc(len_size);

	FILE *file = fopen(filename, "rb");
	int doLandmark = true;

	int *pResults = NULL;
	while (true) {
		int n = fread(buffer, 1, len_size, file);
		if (n <= 0) {
			break;
		}

		cv::Mat l_image(height + height / 2, width, CV_8UC1, buffer);
		Mat result_frontal = l_image.clone();
		
		cv::Mat gray;
		cv::cvtColor(l_image, gray, cv::COLOR_YUV420p2GRAY);

		time_t start_t = get_time_ms();

		pResults = facedetect_frontal((unsigned char*)buffer, (unsigned char*)(gray.ptr(0)), gray.cols, gray.rows, (int)gray.step,
			1.2f, 2, 48, 0, doLandmark);

		time_t stop_t = get_time_ms();

		printf("%d faces detected. waste time %d\n", (pResults ? *pResults : 0), stop_t - start_t);
		
		for (int i = 0; i < (pResults ? *pResults : 0); i++)
		{
			short * p = ((short*)(pResults + 1)) + 142 * i;
			int x = p[0];
			int y = p[1];
			int w = p[2];
			int h = p[3];
			int neighbors = p[4];
			int angle = p[5];

			printf("face_rect=[%d, %d, %d, %d], neighbors=%d, angle=%d\n", x, y, w, h, neighbors, angle);
			rectangle(result_frontal, Rect(x, y, w, h), Scalar(0, 255, 0), 2);
			if (doLandmark)
			{
				for (int j = 0; j < 68; j++)
					circle(result_frontal, Point((int)p[6 + 2 * j], (int)p[6 + 2 * j + 1]), 1, Scalar(0, 255, 0));
			}
		}
		imshow("video", result_frontal);
		waitKey(20);
	}
	if (file) {
		fclose(file);
	}
	while (true) {
		::Sleep(20);
	}

	return 0;
}