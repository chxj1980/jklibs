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

private:

};

#endif
