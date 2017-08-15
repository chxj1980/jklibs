
#include "stdafx.h"
#include "DrawSomething.h"

DrawSomething::DrawSomething() {}

DrawSomething::~DrawSomething() {}

void DrawSomething::make_one_draw()
{
	cv::Mat atomImage = cv::Mat::zeros(WINDOW_WIDTH, WINDOW_WIDTH, CV_8UC3);
	cv::Mat rookImage = cv::Mat::zeros(WINDOW_WIDTH, WINDOW_WIDTH, CV_8UC3);

	DrawEllipse(atomImage, 90);
	DrawEllipse(atomImage, 0);
	DrawEllipse(atomImage, 45);
	DrawEllipse(atomImage, -45);

	DrawFilledCircle(atomImage, cv::Point(WINDOW_WIDTH / 2, WINDOW_WIDTH / 2));

	DrawPolygon(rookImage);

	cv::rectangle(rookImage, cv::Point(0, 7 * WINDOW_WIDTH / 8), cv::Point(WINDOW_WIDTH, WINDOW_WIDTH), cv::Scalar(0, 255, 255), -1, 8);

	DrawLine(rookImage, cv::Point(0, 15 * WINDOW_WIDTH / 16), cv::Point(WINDOW_WIDTH, 15 * WINDOW_WIDTH / 16));
	DrawLine(rookImage, cv::Point(WINDOW_WIDTH / 4, 7 * WINDOW_WIDTH / 8), cv::Point(WINDOW_WIDTH / 4, WINDOW_WIDTH));
	DrawLine(rookImage, cv::Point(WINDOW_WIDTH / 2, 7 * WINDOW_WIDTH / 8), cv::Point(WINDOW_WIDTH / 2, WINDOW_WIDTH));
	DrawLine(rookImage, cv::Point(3 * WINDOW_WIDTH / 4, 7 * WINDOW_WIDTH / 8), cv::Point(3 * WINDOW_WIDTH / 4, WINDOW_WIDTH));

	cv::imshow("draw1", atomImage);
	cv::moveWindow("draw1", 0, 200);
	cv::imshow("draw2", rookImage);
	cv::moveWindow("draw2", WINDOW_WIDTH, 200);

	cv::waitKey(0);
}

void DrawSomething::make_img_reduce(const char *imgname)
{
	cv::Mat srcImage = cv::imread(imgname);
	cv::imshow("original image: ", srcImage);

	cv::Mat dstImage;
	dstImage.create(srcImage.rows, srcImage.cols, srcImage.type());

	double time0 = static_cast<double>(cv::getTickCount());

	ColorReduce(srcImage, dstImage, 32);

	time0 = ((double)cv::getTickCount() - time0) / cv::getTickFrequency();
	printf("Usage time: %lf\n", time0);

	cv::imshow("result image: ", dstImage);
	cv::waitKey(0);
}
