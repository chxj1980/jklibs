#pragma once

#ifndef __BASE_OPERATION_HEADER
#define __BASE_OPERATION_HEADER

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
