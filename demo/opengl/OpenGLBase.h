
#pragma once

#ifndef __OPENGL_BASE_HEADER_
#define __OPENGL_BASE_HEADER_

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
