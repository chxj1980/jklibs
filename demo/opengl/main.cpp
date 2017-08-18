/**
* 最简单的OpenGL播放视频的例子（OpenGL播放YUV）[Texture]
* Simplest Video Play OpenGL (OpenGL play YUV) [Texture]
*
* 雷霄骅 Lei Xiaohua
* leixiaohua1020@126.com
* 中国传媒大学/数字电视技术
* Communication University of China / Digital TV Technology
* http://blog.csdn.net/leixiaohua1020
*
* 本程序使用OpenGL播放YUV视频像素数据。本程序支持YUV420P的
* 像素数据作为输入，经过转换后输出到屏幕上。其中用到了多种
* 技术，例如Texture，Shader等，是一个相对比较复杂的例子。
* 适合有一定OpenGL基础的初学者学习。
*
* 函数调用步骤如下：
*
* [初始化]
* glutInit()：初始化glut库。
* glutInitDisplayMode()：设置显示模式。
* glutCreateWindow()：创建一个窗口。
* glewInit()：初始化glew库。
* glutDisplayFunc()：设置绘图函数（重绘的时候调用）。
* glutTimerFunc()：设置定时器。
* InitShaders()：设置Shader。包含了一系列函数，暂不列出。
* glutMainLoop()：进入消息循环。
*
* [循环渲染数据]
* glActiveTexture()：
* glBindTexture()：
* glTexImage2D()：
* glUniform1i()：
* glDrawArrays():绘制。
* glutSwapBuffers()：显示。
*
* This software plays YUV raw video data using OpenGL.
* It support read YUV420P raw file and show it on the screen.
* It's use a slightly more complex technologies such as Texture,
* Shaders etc. Suitable for beginner who already has some
* knowledge about OpenGL.
*
* The process is shown as follows:
*
* [Init]
* glutInit(): Init glut library.
* glutInitDisplayMode(): Set display mode.
* glutCreateWindow(): Create a window.
* glewInit(): Init glew library.
* glutDisplayFunc(): Set the display callback.
* glutTimerFunc(): Set timer.
* InitShaders(): Set Shader, Init Texture. It contains some functions about Shader.
* glutMainLoop(): Start message loop.
*
* [Loop to Render data]
* glActiveTexture(): Active a Texture
* glBindTexture(): Bind Texture
* glTexImage2D():
* glUniform1i():
* glDrawArrays(): draw.
* glutSwapBuffers(): show.
*/

#include "stdafx.h"

#include "OpenGLGLUT.h"

#include "map"
#include "OpenGLQt.h"

std::map<std::string, std::string> cfg_data;

int read_config(const char *file, std::map<std::string, std::string> &data)
{
	FILE *f = NULL;
#ifdef _WIN32
	fopen_s(&f, file, "rb");
#else
	f = fopen(file, "r");
#endif
	if (f)
	{
		char line[128] = { 0 };
		while (true)
		{
			if (feof(f)) break;
			char *p = fgets(line, 128, f);
			if (!p)
			{
				break;
			}
			if (line[0] == '\0' || line[0] == '#')
			{
				continue;
			}
			char key[64] = { 0 };
			char value[128] = { 0 };
#ifdef _WIN32
			char *sl = line;
			char *tosave = key;
			while (true)
			{
				if (*sl == '\0') break;
				if (*sl == '\r' || *sl == '\n')
				{
					sl++;
					continue;
				}
				if (*sl == '=')
				{
					tosave = value;
					sl++;
				}
				*tosave++ = *sl++;

			}
			int ret = 2;
			//			int ret = sscanf_s(line, "%[^=]=%s", key, value);
#else
			int ret = sscanf(line, "%[^=]=%s", key, value);
#endif
			if (ret == 2)
			{
				data.insert(std::make_pair(key, value));
			}
		}
	}

	return 0;
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		LOG("Usage: %s config_file\n", argv[0]);
		return 1;
	}

	read_config(argv[1], cfg_data);
	const char *cmd = cfg_data["cmd"].c_str();

	if (strcmp(cmd, "glut") == 0)
	{
		OpenGLGLUT *base = new OpenGLGLUT(atoi(cfg_data["yuv_width"].c_str()), atoi(cfg_data["yuv_height"].c_str()));
		base->create_window(atoi(cfg_data["w_width"].c_str()), atoi(cfg_data["w_height"].c_str()));
		base->read_file_start(cfg_data["yuv_file"].c_str());
	}
	else if (strcmp(cmd, "qt") == 0)
	{
		OpenGLQt *base = new OpenGLQt(atoi(cfg_data["yuv_width"].c_str()), atoi(cfg_data["yuv_height"].c_str()));
		base->create_window(atoi(cfg_data["w_width"].c_str()), atoi(cfg_data["w_height"].c_str()));
		base->read_file_start(cfg_data["yuv_file"].c_str());
	}

	getchar();
	return 0;
}
