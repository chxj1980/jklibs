
#include "stdafx.h"
#include "OpenGLQt.h"

static char *args[] = { "OpenGL" };

OpenGLQt::OpenGLQt(int width, int height)
{
	m_width = width;
	m_height = height;
	m_argc = 1;
	m_file = NULL;

	m_qt = new QApplication(m_argc, args);
	m_widget = new QWidget();
	m_v_widget = new OpenGLQtWidget(m_widget);
}

OpenGLQt::~OpenGLQt()
{
	
}

int OpenGLQt::create_window(int width, int height)
{
	m_widget->resize(width, height);
	m_widget->show();
	m_v_widget->initializeGL();
	m_v_widget->resize(width, height);
	m_v_widget->resizeGL(width, height);
	m_v_widget->show();
	
	return 0;
}

int OpenGLQt::read_file_start(const char* filename)
{
	//Open YUV420P file
#ifdef _WIN32
	fopen_s(&m_file, filename, "rb");
#else
	m_file = fopen(filename, "rb");
#endif
	if (m_file == NULL) {
		LOG("cannot open this file [%s]\n", filename);
		return -1;
	}

    int size = m_width * m_height * 3 / 2;
	unsigned char *buf = new unsigned char[size];
	while(true)
	{
		if (fread(buf, 1, m_width*m_height * 3 / 2, m_file) != m_width*m_height * 3 / 2) {
			// Loop
			fseek(m_file, 0, SEEK_SET);
			fread(buf, 1, m_width*m_height * 3 / 2, m_file);
		}

		m_v_widget->set_data(buf, m_width, m_height);

		m_qt->eventDispatcher();
		m_qt->processEvents();
		sys_sleep(40);
	}
	
	return 0;
}

