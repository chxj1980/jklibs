
#include "OpenGLGLUT.h"

static FILE *m_file = NULL;
static unsigned char *buf = NULL;
static int s_width = 0;
static int s_height = 0;
// static char *format = "yuv420p";
static char *format = "yuv422";
static int loop = 0;

static OpenGLBase *base_;

int get_size(const char *format)
{
	if (strcmp(format, "yuv420p") == 0) {
		return s_width * s_height * 3 / 2;
	} else if (strcmp(format, "yuv422") == 0) {
		return s_width * s_height * 2;
	} else if (strcmp(format, "yuv444") == 0) {
		return s_width * s_height * 3;
	}
	return 0;
}

OpenGLGLUT::OpenGLGLUT(int width, int height)
{
	m_argc = 1;
	base_ = this;
	s_width = width;
	s_height = height;
	int size = get_size(format);
	buf = new unsigned char[size];
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
	int size = get_size(format);
	if (fread(buf, 1, size, m_file) != size && loop == 1) {
		// Loop
		fseek(m_file, 0, SEEK_SET);
		fread(buf, 1, size, m_file);
	}
	base_->play(buf, s_width, s_height, format);
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


