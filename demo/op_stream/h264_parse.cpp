//
// Created by v on 16-12-22.
//
#include <stdio.h>
#include <string.h>
#include "h264.h"

char *find_it(char *start, int len, char v)
{
    if (len <= 3 || ! start) return NULL;
    char *p = start;
    char *p_end = &start[len];
    while (p < p_end -3) {
        if (*p == 0x00 && *(p+1) == 0x00 && *(p+2) == 0x00 && *(p+3) == 0x01) {
            if (*(p+4) == v) {
                break;
            }
        }
        p++;
    }
    return p;
}

char *find_sps(char *start, int len)
{
    return find_it(start, len, 0x67);
}

char *find_pps(char *start, int len)
{
    return find_it(start, len, 0x68);
}

int parser_resolution(char *start, int len, char *output, int *outputlen)
{
    if (!output) return -1;

    char *sps = find_sps(start, len);
    char *pps = find_pps(sps, len);
    if (sps && pps) {
        int sps_len = pps - sps;
        if (sps_len <= 0) return -2;
        memcpy(output, sps, sps_len);
        if (outputlen) *outputlen = sps_len;
    }
    return 0;
}

int read_data(const char *filepath, char *data, int *datalen)
{
    if (!data || !filepath || ! datalen) return -1;
    FILE *file = fopen(filepath, "r");
    if (!file ) return -2;

    int ret = fread(data, 1, *datalen, file);
    if (ret <= 0) {
        fclose(file);
        return -3;
    }
    *datalen = ret;

    fclose(file);
    return *datalen;
}

int main(int argc, char **args)
{
    if (argc < 2) {
        printf("Usage: %s h264_file\n", args[0]);
        return -1;
    }
    char *file_name = args[1];

    char inputdata[1024] = {0};
    int inputlen = 1024;

    int ret = read_data(file_name, inputdata, &inputlen);
    if (ret < 0) {
        printf("Error: read file failed [%d]\n", ret);
        return -2;
    }

    char output[1024] = {0};
    int  outputlen = 0;

    ret = parser_resolution(inputdata, inputlen, output, &outputlen);
    if (ret < 0) {
        printf("Error: parser resolution failed [%d]\n", ret);
        return -3;
    }
    video_format_t format;
    block_t block;
    block.i_buffer = 1024;
    block.p_buffer = output;

    format = spsparser(&block);
    printf("[%d,%d]\n", format.i_width, format.i_height);

    return 0;
}