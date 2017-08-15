
#include "stdafx.h"
#include "BaseOperation.h"

BaseOperation::BaseOperation()
{
}

BaseOperation::~BaseOperation()
{
}

void BaseOperation::DrawEllipse(cv::Mat img, double angle)
{
	int thickness = 2;
	int lineType = 8;
	cv::ellipse(img, cv::Point(WINDOW_WIDTH / 2, WINDOW_WIDTH / 2),
		cv::Size(WINDOW_WIDTH / 4, WINDOW_WIDTH / 16),
		angle, 0, 360, cv::Scalar(255, 129, 0), thickness, lineType);
}

void BaseOperation::DrawFilledCircle(cv::Mat img, cv::Point center)
{
	int thickness = -1;
	int lineType = 8;
	cv::circle(img, center, WINDOW_WIDTH / 32, cv::Scalar(0, 0, 255),
		thickness, lineType);
}

void BaseOperation::DrawPolygon(cv::Mat img)
{
	int lineType = 8;

	cv::Point rookPoints[1][20];
	rookPoints[0][0] = cv::Point(WINDOW_WIDTH / 4, 7 * WINDOW_WIDTH / 8);
	rookPoints[0][1] = cv::Point(3 * WINDOW_WIDTH / 4, 7 * WINDOW_WIDTH / 8);
	rookPoints[0][2] = cv::Point(3 * WINDOW_WIDTH / 4, 13 * WINDOW_WIDTH / 16);
	rookPoints[0][3] = cv::Point(11 * WINDOW_WIDTH / 16, 13 * WINDOW_WIDTH / 16);
	rookPoints[0][4] = cv::Point(19 * WINDOW_WIDTH / 32, 3 * WINDOW_WIDTH / 8);
	rookPoints[0][5] = cv::Point(3 * WINDOW_WIDTH / 4, 3 * WINDOW_WIDTH / 8);
	rookPoints[0][6] = cv::Point(3 * WINDOW_WIDTH / 4, WINDOW_WIDTH / 8);
	rookPoints[0][7] = cv::Point(26 * WINDOW_WIDTH / 40, WINDOW_WIDTH / 8);
	rookPoints[0][8] = cv::Point(26 * WINDOW_WIDTH / 40, WINDOW_WIDTH / 4);
	rookPoints[0][9] = cv::Point(22 * WINDOW_WIDTH / 40, WINDOW_WIDTH / 4);
	rookPoints[0][10] = cv::Point(22 * WINDOW_WIDTH / 40, WINDOW_WIDTH / 8);
	rookPoints[0][11] = cv::Point(18 * WINDOW_WIDTH / 40, WINDOW_WIDTH / 8);
	rookPoints[0][12] = cv::Point(18 * WINDOW_WIDTH / 40, WINDOW_WIDTH / 4);
	rookPoints[0][13] = cv::Point(14 * WINDOW_WIDTH / 40, WINDOW_WIDTH / 4);
	rookPoints[0][14] = cv::Point(14 * WINDOW_WIDTH / 40, WINDOW_WIDTH / 8);
	rookPoints[0][15] = cv::Point(WINDOW_WIDTH / 4, WINDOW_WIDTH / 8);
	rookPoints[0][16] = cv::Point(WINDOW_WIDTH / 4, WINDOW_WIDTH / 8);
	rookPoints[0][17] = cv::Point(13 * WINDOW_WIDTH / 32, 3 * WINDOW_WIDTH / 8);
	rookPoints[0][18] = cv::Point(5 * WINDOW_WIDTH / 16, 13 * WINDOW_WIDTH / 16);
	rookPoints[0][19] = cv::Point(WINDOW_WIDTH / 4, 13 * WINDOW_WIDTH / 16);

	const cv::Point *ppt[1] = { rookPoints[0] };
	int npt[] = { 20 };

	cv::fillPoly(img, ppt, npt, 1, cv::Scalar(255, 255, 255), lineType);
}

void BaseOperation::DrawLine(cv::Mat img, cv::Point start, cv::Point end)
{
	int thickness = 2;
	int lineType = 8;

	cv::line(img, start, end, cv::Scalar(0, 0, 0), thickness, lineType);
}

void BaseOperation::ColorReduce(cv::Mat &inputImage, cv::Mat &outputImage, int div)
{
	outputImage = inputImage.clone();

	cv::Mat_<cv::Vec3b>::iterator it = outputImage.begin<cv::Vec3b>();
	cv::Mat_<cv::Vec3b>::iterator itend = outputImage.end<cv::Vec3b>();

	for (; it != itend; ++it)
	{
		for (int i = 0; i < 3; i++)
		{
			(*it)[i] = (*it)[i] / div*div + div / 2;
		}
	}
}
