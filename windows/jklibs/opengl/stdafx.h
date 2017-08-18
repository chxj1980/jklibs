// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

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
#pragma comment(lib, "glut32.lib")
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


// TODO: reference additional headers your program requires here
