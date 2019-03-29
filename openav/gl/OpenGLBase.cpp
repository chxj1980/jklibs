

#include "OpenGLBase.h"

#define ATTRIB_VERTEX 3
#define ATTRIB_TEXTURE 4

//Select one of the Texture mode (Set '1'):
#define TEXTURE_DEFAULT   1
//Rotate the texture
#define TEXTURE_ROTATE    0
//Show half of the Texture
#define TEXTURE_HALF      0

OpenGLBase::OpenGLBase()
{
}

OpenGLBase::~OpenGLBase()
{
}


char *textFileRead(char * filename)
{
	char *s = (char *)malloc(8000);
	memset(s, 0, 8000);
	FILE *infile = fopen(filename, "rb");
	int len = fread(s, 1, 8000, infile);
	fclose(infile);
	s[len] = 0;
	return s;
}

int OpenGLBase::init()
{
	//GLenum gl = glewInit();
	//LOG("GL: %d\n", gl);
	GLint vertCompiled, fragCompiled, linked;

	GLint v, f;
	char *vs, *fs;

	//Shader: step1
	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);
	//Get source code
	vs = textFileRead("Shader.vsh");
	fs = textFileRead("Shader.fsh");
	//Shader: step2
	glShaderSource(v, 1, &vs, NULL);
	glShaderSource(f, 1, &fs, NULL);
	//Shader: step3
	glCompileShader(v);
	//Debug
	glGetShaderiv(v, GL_COMPILE_STATUS, &vertCompiled);
	glCompileShader(f);
	glGetShaderiv(f, GL_COMPILE_STATUS, &fragCompiled);

	//Program: Step1
	p = glCreateProgram();
	//Program: Step2
	glAttachShader(p, v);
	glAttachShader(p, f);

	glBindAttribLocation(p, ATTRIB_VERTEX, "vertexIn");
	glBindAttribLocation(p, ATTRIB_TEXTURE, "textureIn");
	//Program: Step3
	glLinkProgram(p);
	//Debug
	glGetProgramiv(p, GL_LINK_STATUS, &linked);
	//Program: Step4
	glUseProgram(p);

	// we can free them when link done.
	if (vs) {
		free(vs);
	}
	if (fs) {
		free(fs);
	}
	glDeleteShader(v);
	glDeleteShader(f);

	//Get Uniform Variables Location
	textureUniformY = glGetUniformLocation(p, "tex_y");
	textureUniformU = glGetUniformLocation(p, "tex_u");
	textureUniformV = glGetUniformLocation(p, "tex_v");

#if TEXTURE_ROTATE
	static const GLfloat vertexVertices[] = {
		-1.0f, -0.5f,
		0.5f, -1.0f,
		-0.5f,  1.0f,
		1.0f,  0.5f,
	};
#else
	static const GLfloat vertexVertices[] = {
		-1.0f, -1.0f,
		1.0f, -1.0f,
		-1.0f,  1.0f,
		1.0f,  1.0f,
	};
#endif

#if TEXTURE_HALF
	static const GLfloat textureVertices[] = {
		0.0f,  1.0f,
		0.5f,  1.0f,
		0.0f,  0.0f,
		0.5f,  0.0f,
	};
#else
	static const GLfloat textureVertices[] = {
		0.0f,  1.0f,
		1.0f,  1.0f,
		0.0f,  0.0f,
		1.0f,  0.0f,
	};
#endif
	//Set Arrays
	glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, vertexVertices);
	//Enable it
	glEnableVertexAttribArray(ATTRIB_VERTEX);
	glVertexAttribPointer(ATTRIB_TEXTURE, 2, GL_FLOAT, 0, 0, textureVertices);
	glEnableVertexAttribArray(ATTRIB_TEXTURE);


	//Init Texture
	glGenTextures(1, &id_y);
	glBindTexture(GL_TEXTURE_2D, id_y);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &id_u);
	glBindTexture(GL_TEXTURE_2D, id_u);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &id_v);
	glBindTexture(GL_TEXTURE_2D, id_v);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return 0;
}


void OpenGLBase::play(unsigned char* data, int width, int height, const char *format)
{
	//YUV Data
	plane[0] = (unsigned char*)data;
	plane[1] = plane[0] + width*height;
	int uvx = width / 2;
	int uvy = height / 2;
	if (strcmp(format, "yuv420p") == 0) {
		plane[2] = plane[1] + width*height / 4;
	} else if (strcmp(format, "yuv422") == 0) {
		plane[2] = plane[1] + width*height / 2;
	} else if (strcmp(format, "yuv444") == 0) {
		plane[2] = plane[1] + width*height;
	} else {
		return ;
	}

	//Clear
	glClearColor(0.0, 255, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	//Y
	//
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, id_y);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, plane[0]);

	glUniform1i(textureUniformY, 0);
	//U
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, id_u);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width / 2, height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, plane[1]);
	glUniform1i(textureUniformU, 1);
	//V
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, id_v);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width / 2, height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, plane[2]);
	glUniform1i(textureUniformV, 2);

	// Draw
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	// Show
	//Double
#ifdef __OPENGL_GLUT
	glutSwapBuffers();
#endif
	//Single
	//glFlush();
}


