// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#ifdef _WIN32
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
#endif


// TODO: reference additional headers your program requires here
