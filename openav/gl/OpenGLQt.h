#pragma once

#ifndef __OPENGL_QT_HEADER_
#define __OPENGL_QT_HEADER_

#ifdef _WIN32
#ifdef _DEBUG
#pragma comment(lib, "Qt5Cored.lib")
#pragma comment(lib, "Qt5Guid.lib")
#pragma comment(lib, "Qt5Widgetsd.lib")
#pragma comment(lib, "Qt5OpenGLd.lib")
#else
#pragma comment(lib, "Qt5Core.lib")
#pragma comment(lib, "Qt5Gui.lib")
#pragma comment(lib, "Qt5Widgets.lib")
#endif
#endif

#include "OpenGLBase.h"
#include "OpenGLQtWidget.h"

#include <QtWidgets/QApplication>

class OpenGLQt
{
public:
	OpenGLQt(int width, int height);
	virtual ~OpenGLQt();

	int create_window(int width, int height);
	int read_file_start(const char *filename);

private:
	int m_width;
	int m_height;
	int m_argc;
	char m_args[4];

	FILE    *m_file;

	OpenGLQtWidget       *m_v_widget;
	QWidget              *m_widget;

	QApplication *m_qt;
};

#endif
