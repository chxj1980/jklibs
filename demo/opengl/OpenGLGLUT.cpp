
#include "stdafx.h"
#include "OpenGLGLUT.h"

static FILE *m_file = NULL;
static unsigned char *buf = NULL;
static int s_width = 0;
static int s_height = 0;

static OpenGLBase *base_;

OpenGLGLUT::OpenGLGLUT(int width, int height)
{
	m_argc = 1;
	base_ = this;
	s_width = width;
	s_height = height;
	buf = new unsigned char[s_width*s_height* 3 / 2];
}

OpenGLGLUT::~OpenGLGLUT()
{
	if (buf)
	{
		delete[]buf;
	}
}

void OpenGLGLUT::display(void)
{
	if (!m_file) return;
	if (fread(buf, 1, s_width*s_height * 3 / 2, m_file) != s_width*s_height * 3 / 2) {
		// Loop
		fseek(m_file, 0, SEEK_SET);
		fread(buf, 1, s_width*s_height * 3 / 2, m_file);
	}
	base_->play(buf, s_width, s_height);
}

void OpenGLGLUT::timeFunc(int value)
{
	display();
	
	// Timer: 40ms
	glutTimerFunc(40, timeFunc, 0);
}

static char *argv[] = { "opengl", NULL };
int OpenGLGLUT::create_window(int width, int height)
{
	//Init GLUT
	glutInit(&m_argc, argv);
	//GLUT_DOUBLE
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA /*| GLUT_STENCIL | GLUT_DEPTH*/);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("Simplest Video Play OpenGL");
	printf("Lei Xiaohua\n");
	printf("http://blog.csdn.net/leixiaohua1020\n");
	printf("Version: %s\n", glGetString(GL_VERSION));
	GLenum l = glewInit();

	glutDisplayFunc(&display);
	glutTimerFunc(40, timeFunc, 0);

	init();

	return 0;
}

int OpenGLGLUT::read_file_start(const char* filename)
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

	glutMainLoop();
	
	return 0;
}


