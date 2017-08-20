
#pragma once

#ifndef __OPENGL_BASE_HEADER_
#define __OPENGL_BASE_HEADER_


#define LOG(fmt, ...)  \
	do {   \
	printf("[%s:%d] => ", __FILE__, __LINE__); \
	printf(fmt, ##__VA_ARGS__); \
	} while (0);

#ifdef _WIN32
#define sys_sleep(x)   ::Sleep(x);
#ifdef _DEBUG
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "GLU32.lib")
#pragma comment(lib, "freeglut.lib")
#pragma comment(lib, "OpenGL32.lib")
#else
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "GLU32.lib")
#pragma comment(lib, "glut32.lib")
#pragma comment(lib, "OpenGL32.lib")
#endif
#else
#include <unisted.h>
#define sys_sleep(x)   usleep(x*1000);
#endif

#ifdef __DARWIN

#include <GL/glew.h>
#include <glut.h>
#else
#include "glew.h"
#include "glut.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#ifndef __DARWIN
#include <malloc.h>
#endif
#include <string.h>

class OpenGLBase
{
public:
	OpenGLBase();
	virtual ~OpenGLBase();

	void play(unsigned char *data, int width, int height);

protected:
	int init();

private:
	unsigned char *plane[3];

	GLuint p;
	GLuint id_y, id_u, id_v; // Texture id
	GLuint textureUniformY, textureUniformU, textureUniformV;
};

#endif
