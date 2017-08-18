#pragma once

#ifndef __OPENGL_QTWIDGET_H
#define __OPENGL_QTWIDGET_H

#include "OpenGLBase.h"
#include <QtWidgets/QWidget>
#include <QtGui/QOpenGLFunctions>
#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLTexture>

class OpenGLQtWidget
	: public QOpenGLWidget, OpenGLBase, protected QOpenGLFunctions
{
public:
	OpenGLQtWidget(QWidget *parent = NULL);
	virtual ~OpenGLQtWidget();

	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();

	void set_data(unsigned char *buf, int width, int height);

private:
	unsigned char *m_buf;
	int            m_width;
	int            m_height;

	bool           m_first_open;

	GLuint textureUniformY; //y纹理数据位置
	GLuint textureUniformU; //u纹理数据位置
	GLuint textureUniformV; //v纹理数据位置
	GLuint id_y; //y纹理对象ID
	GLuint id_u; //u纹理对象ID
	GLuint id_v; //v纹理对象ID
	QOpenGLTexture* m_pTextureY;  //y纹理对象
	QOpenGLTexture* m_pTextureU;  //u纹理对象
	QOpenGLTexture* m_pTextureV;  //v纹理对象
	QOpenGLShader *m_pVSHader;  //顶点着色器程序对象
	QOpenGLShader *m_pFSHader;  //片段着色器对象
	QOpenGLShaderProgram *m_pShaderProgram; //着色器程序容器
};

#endif
