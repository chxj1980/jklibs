
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "codec.h"

int main(int argc, char **args) {
	if (argc < 5) {
		printf("Usage: %s input_file output_file width height\n",
				args[0]);
		exit(0);
	}
    char *input_file = args[1];
    char *output_file = args[2];
	int outtype = 0;
    char *p = rindex(output_file, '.');
	if (p) {
		if (strncmp(p+1, "h265", 4) == 0) {
			outtype = 1;
		}
	}

	int x = atoi(args[3]);
	int y = atoi(args[4]);
	printf("Convert from [%s]-> [%s] with [%d,%d] type [%s]\n", input_file,
			output_file, x, y, outtype == 1 ? "h265" : "h264");
    CodecHandle h;
	int ret = 0;
    ret = CodecInit(&h, CODEC_VIDEO_YUV420, outtype == 1 ? CODEC_VIDEO_H265 : CODEC_VIDEO_H264);
    if (ret < 0) {
        printf("Error codec init [%d]\n", ret);
        return -1;
    }
	int size = x * y * 3/2;

    CodecSetSolution(h, x, y);
    ret = CodecOpen(h);
    if (ret < 0) {
        printf("Error open [%d]\n", ret);
        return -2;
    }

    printf("Codec init success\n");
    FILE *fin = fopen(input_file, "r");
    FILE *fout = fopen(output_file, "w");
    unsigned char *data = (unsigned char*)malloc(size);
    unsigned char *outdata = NULL;
    unsigned int outlength = 0;
	int frames = 0;
    int alllength = 0;
    while (1) {
        int n = fread(data, 1, size, fin);
        if (n <= 0) {
            printf("Read done\n");
            break;
        }
        ret = CodecPush(h, data, size);
        if (ret < 0) {
            printf("Codec error, but conintue [%d]\n", ret);
        } else {
			//printf("push data success len [%d]\n", size);
		}
        CodecPop(h, &outdata, &outlength);
        alllength += outlength;
        printf("Pop out data [%d] alllength [%d]\n", outlength, alllength);
        fwrite(outdata, 1, outlength, fout);
		frames++;
    }

    CodecClose(&h);
    fclose(fin);
    fclose(fout);
	printf("Convert done of frames [%d]\n", frames);

    return 0;
}
