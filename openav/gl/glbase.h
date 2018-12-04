/*********************************************************
 *
 * Filename: openav/gl/glbase.h
 *   Author: jmdvirus
 *   Create: 2018年12月04日 星期二 09时31分30秒
 *
 *********************************************************/

#pragma once

#ifndef __OPENGL_GLBASE_HEADER_
#define __OPENGL_GLBASE_HEADER_

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
#include <unistd.h>
#define sys_sleep(x)   usleep(x*1000);
#endif

#ifdef __DARWIN

#include <GL/glew.h>
#include <glut.h>
#else
#include "GL/glew.h"
#ifdef __OPENGL_GLUT
#include "GL/glut.h"
#endif
#include <GLFW/glfw3.h>
#endif

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _CMGLBase {
	GLchar          *szVertexSource;
	GLchar          *szFragmentSource;

	GLFWwindow    *window;
	GLuint         shaderProgram;
	GLuint         VAO;
	GLuint         VBO;
} CMGLBase;

int cm_gl_base_init(CMGLBase *gl);

int cm_gl_base_deinit(CMGLBase *gl);

int cm_gl_base_source_file(CMGLBase *gl, const char *vs, const char *fs);

int cm_gl_base_draw_init(CMGLBase *gl);

int cm_gl_base_window(CMGLBase *gl, int w, int h);

// run cycle
// not block
// caller call always
int cm_gl_base_run(CMGLBase *gl);

int cm_gl_base_window_close(CMGLBase *gl);

#ifdef __cplusplus
}
#endif

#endif
