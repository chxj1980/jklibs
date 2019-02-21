/*********************************************************
 *
 * Filename: openav/catch_imgs.cpp
 *   Author: jmdvirus
 *   Create: 2019年02月20日 星期三 12时47分23秒
 *
 *********************************************************/

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/stitching.hpp"

#include "cm_logprint.h"
#include "cmyuv.h"
using namespace std;
using namespace cv;

CMLogPrint logPrint;

static int width = 640;
static int height = 480;
static char *yuvformat = "yuy2";

int catch_pictures(const char *filename)
{
	FILE *f = fopen(filename, "r");
	if (!f) {
		printf("Error: open %s error \n", filename);
		return -2;
	}

	int inlen = width * height * 2;
	int dstlen = width * height * 3/2;

	char *data = (char*)malloc(inlen);
	char *dstdata = (char*)malloc(dstlen);
	int convert = 1;
#if 1

    if (strcmp(yuvformat, "yuy2") == 0) {
        // origin
	} else if (strcmp(yuvformat, "yuv420p") == 0) {
		printf("format is yuv420p\n");
		inlen = width * height * 3/2;
		free(dstdata);
		data = (char*)realloc(data, inlen);
		convert = 0;
	}
#endif

	int frames = 0;

	while (1) {
		int ret = fread(data, 1, inlen, f);
		if (ret != inlen) {
			if (ret == 0) {
				printf("Read done.\n");
				break;
			} else {
				printf("Error: read error ret [%d]\n", ret);
				break;
			}
		}

		if (convert) {
			cm_yuy2_yuv420p(data, width, height, dstdata);
		}

		Mat yuvImgt, yuvImg;
		yuvImgt.create(height * 3/2, width, CV_8UC1);
		memcpy(yuvImgt.data, dstdata, dstlen);
		cvtColor(yuvImgt, yuvImg, cv::COLOR_YUV420p2RGB);

		char name[32] = {0};
		sprintf(name, "o/pic-%d.jpg", frames);
		printf("Generate img [%s]\n", name);
		imwrite(name, yuvImg);
		frames++;
	}

	if (f) {
		fclose(f);
	}
	if (data) free(data);
	if (dstdata) free(dstdata);

	return 0;
}

int main(int argc, char **args) {
	if (argc < 2) {
		printf("Usage: %s filename\n", argc);
		printf("  take out frame and convert to pictures from yuv files\n");
		printf("  support yuy2 only now\n");
		printf("  save to o, remember mkdir o\n");
		return 0;
	}
	const char *yuvfile = args[1];
	printf("Start to catch pictures from [%s]\n", yuvfile);

	if (access(yuvfile, F_OK) != 0) {
		printf("Error: %s not exist\n", yuvfile);
		return -1;
	}

    catch_pictures(yuvfile);
	return 0;
}
