/*********************************************************
 *
 * Filename: codec/enc_h265.c
 *   Author: jmdvirus
 *   Create: 2018年08月24日 星期五 11时08分09秒
 *
 *********************************************************/

#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "x265.h"
#include "enc_h265.h"

typedef struct {
	x265_nal       *pNals;
	uint32_t        iNal;
	x265_param     *pParam;
	x265_encoder   *pEncoder;
	x265_picture   *pPicIn;

	unsigned char   *pData;
	unsigned int     iLen;

	int             iHeight;
	int             iWidth;
	int             icsp;

	int             iYSize;
} EncH265;

int enc_h265_init_default(CMEncoder *enc)
{
	EncH265 *in = (EncH265*)malloc(sizeof(EncH265));
	enc->szPrivate = in;
	memset(in, 0, sizeof(EncH265));
	//EncH265 *in = (EncH265*)enc->szPrivate;
	//if (!in) return -1;

	in->pParam = x265_param_alloc();
	x265_param_default(in->pParam);
	in->pParam->bRepeatHeaders = 1;
	in->pParam->internalCsp = in->icsp;
	in->pParam->fpsNum = 25;
	in->pParam->fpsDenom = 1;
	in->icsp = X265_CSP_I420;

	return 0;
}

int enc_h265_set_format(CMEncoder *enc, int format)
{
	if (!enc) return -1;
	EncH265 *in = (EncH265*)enc->szPrivate;
	if (!in) return -1;

	switch (format) {
		case CODEC_VIDEO_YUV422:
			in->icsp = X264_CSP_I422;
			break;
		case CODEC_VIDEO_YUV420:
			in->icsp = X264_CSP_I420;
			break;
		default:
			break;
	}
	return 0;
}

int enc_h265_set_size(CMEncoder *enc, int w, int h)
{
	if (!enc) return -1;
	EncH265 *in = (EncH265*)enc->szPrivate;
	if (!in) return -1;

	in->iWidth = w;
	in->iHeight = h;
	in->pParam->sourceWidth = w;
	in->pParam->sourceHeight = h;

	in->iYSize = in->pParam->sourceWidth * in->pParam->sourceHeight;

	return 0;
}

int enc_h265_start(CMEncoder *enc)
{
	EncH265 *in = (EncH265*)enc->szPrivate;
	if (!in) return -1;

	if (!in->pParam) return -1;
	if (in->pPicIn) x265_picture_free(in->pPicIn);
	
	in->pPicIn = x265_picture_alloc();
	x265_picture_init(in->pParam, in->pPicIn);

	in->pEncoder = x265_encoder_open(in->pParam);
	if (!in->pEncoder) {
		return -2;
	}
	return 0;
}

static int enc_h265_set_position(EncH265 *enc, unsigned char *data)
{
	EncH265 *in = enc;
	unsigned char *buf = data;
	switch (in->icsp) {
		case X265_CSP_I444:
			{
				in->pPicIn->planes[0] = buf;
				in->pPicIn->planes[1] = buf + in->iYSize;
				in->pPicIn->planes[2] = buf + in->iYSize * 2;
				in->pPicIn->stride[0] = in->iWidth;
				in->pPicIn->stride[1] = in->iWidth;
				in->pPicIn->stride[2] = in->iWidth;
			}
			break;
		case X265_CSP_I420:
			{
				in->pPicIn->planes[0] = buf;
				in->pPicIn->planes[1] = buf + in->iYSize;
				in->pPicIn->planes[2] = buf + in->iYSize * 5 / 4;
				in->pPicIn->stride[0] = in->iWidth;
				in->pPicIn->stride[1] = in->iWidth/2;
				in->pPicIn->stride[2] = in->iWidth/2;
			}
			break;
		default:
			return -3;
			break;
	}
	return 0;
}

int enc_h265_push(CMEncoder *enc, unsigned char *data, unsigned int len)
{
	EncH265 *in = (EncH265*)enc->szPrivate;
	if (!in) return -1;
	if (!enc || !data) return -1;

	int ret = enc_h265_set_position(in, data);
	if (ret < 0) return -2;

	in->iNal = 0;
	ret = x265_encoder_encode(in->pEncoder, &in->pNals, &in->iNal, in->pPicIn, NULL);

	if (ret > 0) {
        int i = 0;
		int allsize = 0;
		for (i = 0; i < in->iNal; i++) {
			allsize += in->pNals[i].sizeBytes;
		}
		if (in->iLen <= allsize) {
			in->pData = (unsigned char *)realloc(in->pData, allsize+1);
		}
		int indx = 0;
		for (i = 0; i < in->iNal; i++) {
			memcpy(in->pData + indx, in->pNals[i].payload, in->pNals[i].sizeBytes);
			indx += in->pNals[i].sizeBytes;
		}
		in->iLen = indx;
	}
	return ret;
}

int enc_h265_pop(CMEncoder *enc, unsigned char **data, unsigned int *len)
{
	EncH265 *in = (EncH265*)enc->szPrivate;
	if (!in) return -1;

	*data = in->pData;
	*len = in->iLen;
	return 0;
}

int enc_h265_stop(CMEncoder *enc)
{
	EncH265 *in = (EncH265*)enc->szPrivate;
	if (!in) return -1;
	if (in->pData) {
		free(in->pData);
	}
	in->pData = NULL;
	in->iLen = 0;
	in->iNal = 0;
	x265_encoder_close(in->pEncoder);
	return 0;
}

int enc_h265_deinit(CMEncoder *enc)
{
	EncH265 *in = (EncH265*)enc->szPrivate;
	if (!in) return -1;

	x265_picture_free(in->pPicIn);
	x265_param_free(in->pParam);

	free(in);
	enc->szPrivate = NULL;
	return 0;
}

CMEncoderHandle CMEncoderH265 = {
	.init = enc_h265_init_default,
	.set_format = enc_h265_set_format,
	.set_size = enc_h265_set_size,
	.start = enc_h265_start,
	.stop = enc_h265_stop,
	.push = enc_h265_push,
	.pop = enc_h265_pop,
	.deinit = enc_h265_deinit,
};

