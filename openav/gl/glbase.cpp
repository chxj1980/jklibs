/*********************************************************
 *
 * Filename: openav/gl/glbase.cpp
 *   Author: jmdvirus
 *   Create: 2018年12月04日 星期二 09时31分22秒
 *
 *********************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "cm_logprint.h"
#include "glbase.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

int cm_gl_base_init(CMGLBase *gl)
{
	memset(gl, 0, sizeof(CMGLBase));
	cmdebug("gl base init success\n");
	return 0;
}

int cm_gl_base_deinit(CMGLBase *gl)
{
	glDeleteVertexArrays(1, &gl->VAO);
	glDeleteBuffers(1, &gl->VBO);
	if (gl->szVertexSource) {
		free(gl->szVertexSource);
	}
	if (gl->szFragmentSource) {
		free(gl->szFragmentSource);
	}
	cmdebug("gl base deinit success\n");
	return 0;
}

int cm_gl_base_source_file(CMGLBase *gl, const char *vs, const char *fs)
{
	if (access(vs, F_OK) != 0 || access(fs, F_OK)) {
		return -1;
	}
	cmdebug("gl base init source file [%s][%s]\n", vs, fs);
	int ret = -2;
	FILE *f = NULL;
	int len = 0;
	struct stat st;
	cmdebug("gl base init vs [%s]\n", vs);
	ret = stat(vs, &st);
	if (ret < 0) {
		goto out;
	}
	gl->szVertexSource = (char*)malloc(st.st_size+1);
	f = fopen(vs, "rb");
	len = fread(gl->szVertexSource, 1, st.st_size, f);
	if (len != st.st_size) {
		ret = -3;
		goto out;
	}
	fclose(f);
	f = NULL;

	cmdebug("gl base init fs [%s]\n", fs);
	ret = stat(fs, &st);
	if (ret < 0) {
		ret = -4;
		goto out;
	}
	gl->szFragmentSource = (char*)malloc(st.st_size+1);
    f = fopen(fs, "rb");
	len = fread(gl->szFragmentSource, 1, st.st_size, f);
	if (len != st.st_size) {
		ret = -5;
		goto out;
	}
	fclose(f);
	f = NULL;
	cmdebug("gl base init source success\n");
	return 0;

out:
	if (f) {
		fclose(f);
	}
	f = NULL;
	if (gl->szVertexSource) {
		free(gl->szVertexSource);
	}
	if (gl->szFragmentSource) {
		free(gl->szFragmentSource);
	}

	return ret;
}

int cm_gl_base_window(CMGLBase *gl, int w, int h)
{
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	gl->window = glfwCreateWindow(w, h, "LearnOpenGL", nullptr, nullptr);
    if (gl->window == nullptr)
    {
		cmerror("glfw create failed\n");
        glfwTerminate();
        return -1;
    }
	glfwMakeContextCurrent(gl->window);
    glfwSetKeyCallback(gl->window, key_callback);

	int width, height;
    glfwGetFramebufferSize(gl->window, &width, &height);
    glViewport(0, 0, width, height);

	cmdebug("gl base window create success with [%dx%d]\n", w, h);
	return 0;
}

int cm_gl_base_window_close(CMGLBase *gl)
{
	glfwTerminate();
	cmdebug("gl base window close success\n");
	return 0;
}

static int cm_gl_base_draw(CMGLBase *gl)
{
	if (!gl) return -1;
    // Clear the colorbuffer
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(gl->shaderProgram);
	glBindVertexArray(gl->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);

	return 0;
}

int cm_gl_base_run(CMGLBase *gl)
{
	if (!gl) return -1;
	if (glfwWindowShouldClose(gl->window)) return -2;
	glfwPollEvents();

	cm_gl_base_draw(gl);

    glfwSwapBuffers(gl->window);

	return 0;
}

int cm_gl_base_draw_init(CMGLBase *gl)
{
	if (!gl) return -1;
    glewExperimental = GL_TRUE;
    glewInit();

	GLfloat vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.0f
	};

	cmdebug("gl base add vertex shader \n");
	GLuint vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &gl->szVertexSource, NULL);
	glCompileShader(vertexShader);

	GLint success;
	GLchar log[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, log);
        cmerror("vertex shader error [%s] \n[%s]\n\n", log, gl->szVertexSource);
	}
	
	cmdebug("gl base init fragment shader\n");

	GLuint fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &gl->szFragmentSource, NULL);
	glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, log);
		cmerror("fragment shader error [%s] \n[%s]\n\n", log, gl->szFragmentSource);
    }

	cmdebug("gl base attach program\n");
	gl->shaderProgram = glCreateProgram();
	glAttachShader(gl->shaderProgram, vertexShader);
	glAttachShader(gl->shaderProgram, fragmentShader);
	glLinkProgram(gl->shaderProgram);
    glGetProgramiv(gl->shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(gl->shaderProgram, 512, NULL, log);
		cmerror("program error [%s]\n", log);
    }

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	cmdebug("gl base init VBO\n");
	glGenBuffers(1, &gl->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, gl->VBO);
	// GL_DYNAMIC_DRAW
	// GL_STREAM_DRAW
	// GL_STATIC_DRAW
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	cmdebug("gl base init VAO\n");

	glGenVertexArrays(1, &gl->VAO);
	glBindVertexArray(gl->VAO);

	cmdebug("gl base enable vertex attrib pointer\n");
	// 1. vec -> location = 0
	// 2. vec3
	// 3. vec3 -> float
	// 4. normalize
	// 5. step to next
	// 6. offset
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

#ifdef GLBASE_TEST
CMLogPrint logPrint;
int main(int argc, char **args) {
	CM_LOG_PRINT_INIT("glbase");
	CMGLBase gl;
	int ret = cm_gl_base_init(&gl);
	if (ret < 0) {
		cmerror("gl base init failed [%d]\n", ret);
		return 125;
	}
	cm_gl_base_source_file(&gl, "v.vsh", "f.fsh");
	cm_gl_base_window(&gl, 800, 600);
	cm_gl_base_draw_init(&gl);

	while (1) {
		ret = cm_gl_base_run(&gl);
		if (ret < 0) break;
		usleep(40000);
	}
	cm_gl_base_window_close(&gl);
	cm_gl_base_deinit(&gl);

	return 0;
}
#endif

