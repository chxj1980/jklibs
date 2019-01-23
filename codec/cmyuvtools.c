/*********************************************************
 *
 * Filename: cmyuvtools.c
 *   Author: jmdvirus
 *   Create: 2019年01月23日 星期三 14时25分31秒
 *
 *********************************************************/

#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <stdlib.h>

#include "cmyuv.h"

int main(int argc, char **args)
{
	if (argc < 5) {
		printf("Usage: %s src dst width height\n", args[0]);
		exit(0);
	}
	const char *src = args[1];
	const char *dst = args[2];
	int width = atoi(args[3]);
	int height = atoi(args[4]);
	printf("Convert from [%s] -> [%s] with [%d, %d]\n", 
			src, dst, width, height);

	int inlen = width * height * 2;
	int outlen = width * height * 3/2;

	char *indata = (char*)malloc(inlen+1);
	char *outdata = (char*)malloc(outlen+1);

	FILE *fin = fopen(src, "r");
	FILE *fout = fopen(dst, "w+");

	int frames = 0;
	while(1) {
	    int ret = fread(indata, 1, inlen, fin);
		if (ret <= 0) {
			printf("Read input file done ret [%d]\n", ret);
			break;
		}
	    if (ret != inlen) {
            printf("read input file done error [%d]\n", ret);
			break;
	    }
		//printf("Read out data [%d]\n", inlen);
		cm_yuy2_yuv420p(indata, width, height, outdata);
		ret = fwrite(outdata, 1, outlen, fout);
        printf("Write outdata [%d]\n", ret);
		frames++;
	}
	fflush(fout);

	printf("Convert done of frames [%d]\n", frames);
	if (fin) fclose(fin);
	if (fout) fclose(fout);
	if (indata) free(indata);
	if (outdata) free(outdata);
	return 0;
}

