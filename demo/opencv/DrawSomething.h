#pragma once

#ifndef __BASE_DRAWSOMETHING
#define __BASE_DRAWSOMETHING
#include "BaseOperation.h"

class DrawSomething
	: public BaseOperation
{
public:
	DrawSomething();
	~DrawSomething();

	void make_one_draw();

	void make_img_reduce(const char *imgname);

	void make_add_roi(const char *img1, const char *img2);

	void make_add_weighted(const char *img1, const char *img2);

	void make_split_merge(const char *img1, const char *img2, const char *img3);

	void make_control_image(const char *img);

	void make_dft(const char *img);

	void make_rotate_image(const char *img);

	void make_rotate_image_i(cv::Mat &img);

private:

};

#endif
