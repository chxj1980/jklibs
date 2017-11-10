/**
 * Create: 2017-11-10
 * Author: jmdvirus
 */

#include <stdint.h>
#include "x264.h"

typedef struct {
    x264_t      *handle;
    x264_param_t   param;
    x264_picture_t   pic_in;
    x264_picture_t   pic_out;
    x264_nal_t       *nal;

    char            *outdata;
    unsigned int     outlength;
} CEncoder;

typedef struct {
} CDecoder;

int init_yuv_h264(CEncoder *en);

int set_solution(CEncoder *en, int h, int w);

int open_yuv_h264(CEncoder *en);

int close_yuv_h264(CEncoder *en);

int encoder_yuv_h264(CEncoder *en, char *data, unsigned int length);
