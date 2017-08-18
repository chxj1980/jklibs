#pragma once

#ifndef __OPENGL_QTWIDGET_H
#define __OPENGL_QTWIDGET_H

#include "OpenGLBase.h"
#ifdef __DARWIN

#include <QtWidgets/QWidget>
#include <QtGui/QOpenGLFunctions>
#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLTexture>
#endif

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

	GLuint textureUniformY; //y��������λ��
	GLuint textureUniformU; //u��������λ��
	GLuint textureUniformV; //v��������λ��
	GLuint id_y; //y�������ID
	GLuint id_u; //u�������ID
	GLuint id_v; //v�������ID
	QOpenGLTexture* m_pTextureY;  //y�������
	QOpenGLTexture* m_pTextureU;  //u�������
	QOpenGLTexture* m_pTextureV;  //v�������
	QOpenGLShader *m_pVSHader;  //������ɫ���������
	QOpenGLShader *m_pFSHader;  //Ƭ����ɫ������
	QOpenGLShaderProgram *m_pShaderProgram; //��ɫ����������
};

#endif
