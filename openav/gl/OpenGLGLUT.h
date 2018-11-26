#pragma once

#ifndef __OPENGL_GLUT_HEADER_
#define __OPENGL_GLUT_HEADER_

#include  "OpenGLBase.h"

class OpenGLGLUT
	: public OpenGLBase
{
public:
	OpenGLGLUT(int width, int height);
	virtual ~OpenGLGLUT();

	int create_window(int width, int height);

	int read_file_start(const char *filename);

	static void display(void);
	static void timeFunc(int value);

private:
	int   m_argc;
};
#endif