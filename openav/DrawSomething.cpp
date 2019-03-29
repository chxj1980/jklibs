
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

void DrawSomething::make_add_roi(const char *img1, const char *img2)
{
	cv::Mat srcImage = cv::imread(img1);
	cv::Mat dstImage = cv::imread(img2);
	if (!srcImage.data || !dstImage.data)
	{
		LOG("Error load image\n");
		return;
	}

	cv::Mat imageROI = srcImage(cv::Rect(20, 30, dstImage.cols, dstImage.rows));

	cv::Mat mask = cv::imread(img2, 0);

	dstImage.copyTo(imageROI, mask);

	cv::namedWindow("ROI image show");
	cv::imshow("ROI Image result: ", srcImage);
	cv::waitKey(0);
}

void DrawSomething::make_add_weighted(const char *img1, const char *img2)
{
	double alphaValue = 0.5;
	double betaValue;

	cv::Mat srcImage1, srcImage2, dstImage;

	srcImage1 = cv::imread(img1);
	srcImage2 = cv::imread(img2);

	if (!srcImage1.data || !srcImage2.data)
	{
		LOG("Error load image\n");
		return;
	}

	betaValue = (1.0 - alphaValue);
	cv::addWeighted(srcImage1, alphaValue, srcImage2, betaValue, 0.0, dstImage);

	cv::imshow("original", srcImage1);
	cv::imshow("result", dstImage);
	cv::waitKey(0);
}

void DrawSomething::make_split_merge(const char *img1, const char *img2, const char *img3)
{
	// img1 and img2 must same size
	// this demo test fail, will check later when have more acknowledge
	cv::Mat image1 = cv::imread(img1);
	cv::Mat image2 = cv::imread(img2);
	cv::Mat image3 = cv::imread(img3);

	if (!image1.data || !image2.data || !image3.data)
	{
		LOG("Error load image\n");
		return;
	}

	std::vector<cv::Mat> channels;
	cv::Mat imageROI;

	cv::split(image1, channels);
	imageROI = channels.at(0);

	cv::addWeighted(imageROI, 1.0,
		image2, 0.5,
		0.0, image1);

	cv::merge(channels, image3);
	cv::imshow("s", image3);
	cv::waitKey(0);
}

int g_contrast_value = 0;
int g_bright_value = 0;
cv::Mat g_src_image, g_dst_image;

static void on_ContrastAndBright(int, void *)
{
	cv::namedWindow("orig", 1);
	for(int y = 0; y < g_src_image.rows; y++)
	{
		for (int x = 0; x < g_src_image.cols; x++)
		{
			for (int c = 0; c < 3; c++)
			{
				g_dst_image.at<cv::Vec3b>(y, x)[c] =
					cv::saturate_cast<uchar>((g_contrast_value*0.01)*(g_src_image.at<cv::Vec3b>(y, x)[c]) + g_bright_value);
			}
		}
	}

	cv::imshow("orig", g_src_image);
	cv::imshow("1", g_dst_image);
}

void DrawSomething::make_control_image(const char *img)
{
	g_src_image = cv::imread(img);
	if (!g_src_image.data)
	{
		LOG("Error load image\n");
		return;
	}

	g_dst_image = cv::Mat::zeros(g_src_image.size(), g_src_image.type());

	g_contrast_value = 80;
	g_bright_value = 0;

	cv::namedWindow("1");
	cv::createTrackbar("contrast: ", "1", &g_contrast_value, 300, on_ContrastAndBright);
	cv::createTrackbar("bright: ", "1", &g_bright_value, 200, on_ContrastAndBright);

	on_ContrastAndBright(g_contrast_value, 0);
	on_ContrastAndBright(g_bright_value, 0);

	while (char(cv::waitKey(1)) != 'q') {}

	return;
}

void DrawSomething::make_dft(const char *img)
{
	cv::Mat image = cv::imread(img);
	if (!image.data)
	{
		LOG("Error load image\n");
		return;
	}

	int m = cv::getOptimalDFTSize(image.rows);
	int n = cv::getOptimalDFTSize(image.cols);

	cv::Mat padded;
	cv::copyMakeBorder(image, padded, 0, m - image.rows, 0, n - image.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));

//	cv::Mat planes[] = { cv::Mat_<float>(padded), cv::Mat::zeros(padded.size(), CV_32F) };
	cv::Mat planes[] = { padded, cv::Mat::zeros(padded.size(), CV_32F) };

	cv::Mat complexI;
	cv::merge(planes, 2, complexI);

	cv::dft(complexI, complexI);

	cv::split(complexI, planes); // planes[0] = cv::Re(DFT(I), planes[1] = Im(DFT(I))))
	cv::magnitude(planes[0], planes[1], planes[0]);
	cv::Mat magnitudeImage = planes[0];
	
	magnitudeImage += cv::Scalar::all(1);
	cv::log(magnitudeImage, magnitudeImage);

	magnitudeImage = magnitudeImage(cv::Rect(0, 0, magnitudeImage.cols & -2, magnitudeImage.rows & -2));

	int cx = magnitudeImage.cols / 2;
	int cy = magnitudeImage.rows / 2;
	cv::Mat q0(magnitudeImage, cv::Rect(0, 0, cx, cy)); // left top
	cv::Mat q1(magnitudeImage, cv::Rect(cx, 0, cx, cy)); // right top
	cv::Mat q2(magnitudeImage, cv::Rect(0, cy, cx, cy));  // left bottom 
	cv::Mat q3(magnitudeImage, cv::Rect(cx, cy, cx, cy)); // right bottom

	cv::Mat tmp;
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);

	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);

	cv::normalize(magnitudeImage, magnitudeImage, 0, 1, cv::NORM_MINMAX);

	cv::imshow("origin image", image);
	cv::imshow("phase image", magnitudeImage);
	cv::waitKey(0);
}


void DrawSomething::make_rotate_image(const char *img)
{
	cv::Point2f srcTriange[3];
	cv::Point2f dstTriange[3];

	cv::Mat rotMat(2, 3, CV_32FC1);
	cv::Mat warpMat(2, 3, CV_32FC1);
	cv::Mat srcImage, dstImage_warp, dstImage_warp_rotate;

	srcImage = cv::imread(img, 1);
	if (!srcImage.data)
	{
		LOG("Error load image\n");
		return;
	}

	dstImage_warp = cv::Mat::zeros(srcImage.rows, srcImage.cols, srcImage.type());

	srcTriange[0] = cv::Point2f(0, 0);
	srcTriange[1] = cv::Point2f(static_cast<float>(srcImage.cols - 1), 0);
	srcTriange[2] = cv::Point2f(0, static_cast<float>(srcImage.rows - 1));

	dstTriange[0] = cv::Point2f(static_cast<float>(srcImage.cols*0.0), static_cast<float>(srcImage.rows*0.33));
	dstTriange[1] = cv::Point2f(static_cast<float>(srcImage.cols*0.65), static_cast<float>(srcImage.rows*0.35));
	dstTriange[2] = cv::Point2f(static_cast<float>(srcImage.cols*0.15), static_cast<float>(srcImage.rows*0.6));

	warpMat = cv::getAffineTransform(srcTriange, dstTriange);

	cv::warpAffine(srcImage, dstImage_warp, warpMat, dstImage_warp.size());

	cv::Point center = cv::Point(dstImage_warp.cols / 2, dstImage_warp.rows / 2);
	double angle = -30.0;
	double scale = 0.8;

	rotMat = cv::getRotationMatrix2D(center, angle, scale);
	cv::warpAffine(dstImage_warp, dstImage_warp_rotate, rotMat, dstImage_warp.size());

	cv::imshow("x", dstImage_warp);
	cv::waitKey(0);
}

void DrawSomething::make_rotate_image_i(cv::Mat& img)
{
	cv::Point2f srcTriange[3];
	cv::Point2f dstTriange[3];

	cv::Mat rotMat(2, 3, CV_32FC1);
	cv::Mat warpMat(2, 3, CV_32FC1);
	cv::Mat dstImage_warp, dstImage_warp_rotate;

	dstImage_warp = cv::Mat::zeros(img.rows, img.cols, img.type());

	srcTriange[0] = cv::Point2f(0, 0);
	srcTriange[1] = cv::Point2f(static_cast<float>(img.cols - 1), 0);
	srcTriange[2] = cv::Point2f(0, static_cast<float>(img.rows - 1));

	dstTriange[0] = cv::Point2f(static_cast<float>(img.cols*0.0), static_cast<float>(img.rows*0.33));
	dstTriange[1] = cv::Point2f(static_cast<float>(img.cols*0.75), static_cast<float>(img.rows*0.25));
	dstTriange[2] = cv::Point2f(static_cast<float>(img.cols*0.15), static_cast<float>(img.rows*0.6));

	warpMat = cv::getAffineTransform(srcTriange, dstTriange);

	cv::warpAffine(img, dstImage_warp, warpMat, dstImage_warp.size());

	cv::Point center = cv::Point(dstImage_warp.cols / 2, dstImage_warp.rows / 2);
	double angle = -30.0;
	double scale = 0.8;

	rotMat = cv::getRotationMatrix2D(center, angle, scale);
	cv::warpAffine(dstImage_warp, dstImage_warp_rotate, rotMat, dstImage_warp.size());

	dstImage_warp.copyTo(img);
}
