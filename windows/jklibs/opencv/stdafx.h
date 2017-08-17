// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifdef _MINGW
#undef _WIN32
#endif

#ifdef _WIN32
#include "targetver.h"
#endif

#include <stdio.h>
#ifdef _WIN32
#include <tchar.h>

#include <Windows.h>
#endif

#define LOG(fmt, ...)  \
	do {   \
	printf("[%s:%d] => ", __FILE__, __LINE__); \
	printf(fmt, ##__VA_ARGS__); \
	} while (0);

#ifdef _WIN32
#ifdef _DEBUG
#pragma comment(lib, "opencv_core320d.lib")
#pragma comment(lib, "opencv_highgui320d.lib")
#pragma comment(lib, "opencv_videoio320d.lib")
#pragma comment(lib, "opencv_imgproc320d.lib")
#pragma comment(lib, "opencv_imgcodecs320d.lib")
#pragma comment(lib, "opencv_objdetect320d.lib")
#else
#pragma comment(lib, "opencv_core320d.lib")
#pragma comment(lib, "opencv_highgui320d.lib")
#pragma comment(lib, "opencv_videoio320d.lib")
#pragma comment(lib, "opencv_imgproc320d.lib")
#pragma comment(lib, "opencv_imgcodecs320d.lib")
#pragma comment(lib, "opencv_objdetect320d.lib")
#endif
#endif

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"

// TODO: reference additional headers your program requires here
