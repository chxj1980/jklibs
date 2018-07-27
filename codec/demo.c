
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "codec.h"

int main(int argc, char **args) {
    char *input_file = args[1];
    char *output_file = args[2];

	int x = 640;
	int y = 480;
	x = 1280;
	y = 960;
    CodecHandle h;
    int ret = CodecInit(&h, CODEC_VIDEO_YUV422, CODEC_VIDEO_H264);
    //int ret = CodecInit(&h, CODEC_VIDEO_YUV420, CODEC_VIDEO_H264);
    if (ret < 0) {
        printf("Error codec init [%d]\n", ret);
        return -1;
    }

    CodecSetSolution(h, x, y);
    ret = CodecOpen(h);
    if (ret < 0) {
        printf("Error open [%d]\n", ret);
        return -2;
    }

    printf("Codec init success\n");
    FILE *fin = fopen(input_file, "r");
    FILE *fout = fopen(output_file, "w");
    int size = x*y*2;
    char *data = (char*)malloc(size);
    char *outdata = NULL;
    unsigned int outlength = 0;
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
        }
        CodecPop(h, &outdata, &outlength);
        alllength += outlength;
        printf("Pop out data [%d] alllength [%d]\n", outlength, alllength);
        fwrite(outdata, 1, outlength, fout);
    }

    CodecClose(&h);
    fclose(fin);
    fclose(fout);

    return 0;
}
