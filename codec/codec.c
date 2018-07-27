
#include <string.h>
#include <stdlib.h>

#include "codec.h"
#include "base.h"

typedef struct {
    int        iInput;
    int        iOutput;

    CEncoder     *en;
    CDecoder     *de;
} CCodec;

int CodecInit(CodecHandle *handle, int input, int output) {
    CCodec *c = (CCodec*)malloc(sizeof(CCodec));
    memset(c, 0, sizeof(CCodec));
    c->iInput = input;
    c->iOutput = output;
    if ((c->iInput == CODEC_VIDEO_YUV422 ||
			   c->iInput == CODEC_VIDEO_YUV420)	&&
        c->iOutput == CODEC_VIDEO_H264) {
        c->en = (CEncoder*)malloc(sizeof(CEncoder));
        init_yuv_h264(c->en, c->iInput);
    } else {
        free(c);
        return -2;
    }
    *handle = c;
    return 0;
}

int CodecOpen(CodecHandle handle) {
    CCodec *c = handle;
    return open_yuv_h264(c->en);
}

int CodecClose(CodecHandle *handle) {
    CCodec *c = *handle;
    if (c->en) free(c->en);
    if (c->de) free(c->de);
    free(c);
    *handle = NULL;
    return 0;
}

API_PREFIX int CodecSetSolution(CodecHandle handle, int w, int h) {
    CCodec *c = handle;
    set_solution(c->en, h, w);
    return 0;
}

int CodecPush(CodecHandle handle, const char *data, unsigned int length) {
    CCodec *c = handle;
    return encoder_yuv_h264(c->en, (char*)data, length);
}

int CodecPop(CodecHandle handle, char **data, unsigned int *length) {
    CCodec *c = handle;
    *data = c->en->outdata;
    *length = c->en->outlength;
    return 0;
}
