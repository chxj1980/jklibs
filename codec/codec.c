
#include <string.h>
#include <stdlib.h>

#include "codec.h"
#include "enc_h265.h"
#include "base.h"

typedef struct {
    int        iInput;
    int        iOutput;

	CMEncoder          en;
	CMEncoderHandle    *cmen;
} CCodec;

int CodecInit(CodecHandle *handle, int input, int output) {
    CCodec *c = (CCodec*)malloc(sizeof(CCodec));
    memset(c, 0, sizeof(CCodec));
    c->iInput = input;
    c->iOutput = output;
    if ((c->iInput == CODEC_VIDEO_YUV422 ||
			   c->iInput == CODEC_VIDEO_YUV420)	&&
        c->iOutput == CODEC_VIDEO_H264) {
		c->cmen = &CMEncoderH264;
		c->cmen->init(&c->en);
	} else if (c->iOutput == CODEC_VIDEO_H265) {
		c->cmen = &CMEncoderH265;
		if (c->cmen->init) {
			c->cmen->init(&c->en);
		}
    } else {
        free(c);
        return -2;
    }
    *handle = c;
    return 0;
}

int CodecOpen(CodecHandle handle) {
    CCodec *c = handle;
	int ret = -1;
	if (c->cmen->start) {
		ret = c->cmen->start(&c->en);
	}
	return ret;
}

int CodecClose(CodecHandle *handle) {
    CCodec *c = *handle;
	int ret = -1;
	if (c->cmen->stop) {
		c->cmen->stop(&c->en);
	}
    free(c);
    *handle = NULL;
    return 0;
}

API_PREFIX int CodecSetSolution(CodecHandle handle, int w, int h) {
    CCodec *c = handle;
	if (c->cmen->set_size) {
		c->cmen->set_size(&c->en, w, h);
	}
    return 0;
}

int CodecPush(CodecHandle handle, unsigned char *data, unsigned int length) {
    CCodec *c = handle;
	int ret = -1;
	if (c->cmen->push) {
		ret = c->cmen->push(&c->en, data, length);
	}
	return ret;
}

int CodecPop(CodecHandle handle, unsigned char **data, unsigned int *length) {
    CCodec *c = handle;
	int ret = -1;
	if (c->cmen->pop) {
		ret = c->cmen->pop(&c->en, data, length);
	}
    return ret;
}
