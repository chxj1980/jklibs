
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "codec.h"
#include "base.h"

typedef struct {
    x264_t      *handle;
    x264_param_t   param;
    x264_picture_t   pic_in;
    x264_picture_t   pic_out;
    x264_nal_t       *nal;

	int              codec_type;
    unsigned char            *outdata;
    unsigned int     outlength;
} EncH264;

int init_yuv_h264(CMEncoder *enc) {
	EncH264 *en = (EncH264*)malloc(sizeof(EncH264));
	enc->szPrivate = en;

    memset(en, 0, sizeof(EncH264));
    x264_param_default_preset(&en->param, "veryfast", "zerolatency");
    en->param.i_threads = X264_SYNC_LOOKAHEAD_AUTO;
    en->param.i_width = 640;
    en->param.i_height = 480;
    en->param.i_frame_total = 0;
    en->param.i_keyint_max = 10;
    en->param.i_bframe = 5;
    en->param.b_open_gop = 0;
    en->param.i_bframe_pyramid = 0;
    en->param.i_bframe_adaptive = X264_B_ADAPT_TRELLIS;
    en->param.i_log_level = X264_LOG_DEBUG;
    en->param.rc.i_bitrate = 1024*10;
    en->param.i_fps_den = 1;
    en->param.i_fps_num = 25;
    en->param.i_timebase_den = en->param.i_fps_num;
    en->param.i_timebase_num = en->param.i_fps_den;
	en->codec_type = X264_CSP_I420;
    x264_param_apply_profile(&en->param, x264_profile_names[4]);

    return 0;
}

int enc_h264_set_format(CMEncoder *enc, int format)
{
	if (!enc) return -1;
	EncH264 *in = (EncH264*)enc->szPrivate;
	if (!in) return -1;

	switch (format) {
		case CODEC_VIDEO_YUV422:
			in->codec_type = X264_CSP_I422;
			break;
		case CODEC_VIDEO_YUV420:
			in->codec_type = X264_CSP_I420;
			break;
		default:
			break;
	}
	return 0;
}

int set_solution(CMEncoder *enc, int w, int h) {
	EncH264 *en = (EncH264*)enc->szPrivate;
	if (!en) return -1;
    en->param.i_width = w;
    en->param.i_height = h;
    return 0;
}

int open_yuv_h264(CMEncoder *enc) {
	EncH264 *en = (EncH264*)enc->szPrivate;
	if (!en) return -1;

    x264_picture_init(&en->pic_out);
	int i_csp = 0;
    x264_picture_alloc(&en->pic_in, en->codec_type, en->param.i_width, en->param.i_height);
    en->pic_in.img.i_csp = en->codec_type;
    en->pic_in.img.i_plane = 3;
    en->pic_in.i_pts = 1;
    int msize = en->param.i_width * en->param.i_height * 3/2;
    en->outdata = (unsigned char*)malloc(msize);
    en->outlength = 0;

    en->handle = x264_encoder_open(&en->param);
    return en->handle == NULL ? -1 : 0;
}

int close_yuv_h264(CMEncoder *enc) {
	EncH264 *en = (EncH264*)enc->szPrivate;
	if (!en) return -1;

	if (en->outdata) {
		free(en->outdata);
	}
	en->outdata = NULL;
	en->outlength = 0;

    x264_picture_clean(&en->pic_in);
    x264_encoder_close(en->handle);
    return 0;
}

static int encoder_yuv422_h264(EncH264 *en, unsigned char *data, unsigned int length) {
    int   iNals = 0;
    char *y = (char*)en->pic_in.img.plane[0];
    char *u = (char*)en->pic_in.img.plane[1];
    char *v = (char*)en->pic_in.img.plane[2];
    int index_y = 0;
    int index_u = 0;
    int index_v = 0;
    int num = en->param.i_width * en->param.i_height * 2 - 4;
    for (int i = 0; i < num; i = i + 4) {
         *(y + (index_y++)) = *(data+i);
         *(u + (index_u++)) = *(data + i + 1);
         *(y + (index_y++)) = *(data + i + 2);
         *(v + (index_v++)) = *(data + i + 3);
    }
    en->pic_in.i_pts ++;
    en->outlength = 0;
    int frame_size = x264_encoder_encode(en->handle, &en->nal, &iNals, &en->pic_in, &en->pic_out);

    if (frame_size < 0) return -1;
	return iNals;
}

static int encoder_yuv420_h264(EncH264 *en, unsigned char *data, unsigned int length) {
    int   iNals = 0;
	en->pic_in.img.plane[0] = (uint8_t*)data;
	en->pic_in.img.plane[1] = (uint8_t*)data + en->param.i_height*en->param.i_width;
	en->pic_in.img.plane[2] = (uint8_t*)data + en->param.i_height*en->param.i_width + 
		en->param.i_height*en->param.i_width /4;
	//en->pic_in.img.plane[1] = (uint8_t*)data + en->param.i_height;
	//en->pic_in.img.plane[2] = (uint8_t*)data + en->param.i_height*5/4;
    en->pic_in.i_pts ++;
    en->outlength = 0;
    int frame_size = x264_encoder_encode(en->handle, &en->nal, &iNals, &en->pic_in, &en->pic_out);

    if (frame_size < 0) return -1;
	return iNals;
}

int encoder_yuv_h264(CMEncoder *enc, unsigned char *data, unsigned int length) {
	EncH264 *en = (EncH264*)enc->szPrivate;
	if (!en) return -1;
	
	int iNals = 0;
	if (en->codec_type == X264_CSP_I422) {
	    iNals = encoder_yuv422_h264(en, data, length);
	} else if (en->codec_type == X264_CSP_I420) {
	    iNals = encoder_yuv420_h264(en, data, length);
	}
	if (iNals < 0) return -1;

    for (int i = 0; i < iNals; i++) {
         memcpy(en->outdata + en->outlength, en->nal[i].p_payload, en->nal[i].i_payload);
         en->outlength += en->nal[i].i_payload;
    }
    return 0;
}

int enc_h264_pop(CMEncoder *enc, unsigned char **data, unsigned int *len)
{
	EncH264 *en = (EncH264*)enc->szPrivate;
	if (!en) return -1;

    *data = en->outdata;
	*len = en->outlength;

	return 0;
}

int enc_h264_deinit(CMEncoder *enc)
{
	EncH264 *en = (EncH264*)enc->szPrivate;
	if (!en) return -1;

	free(en);
	enc->szPrivate = NULL;
	return 0;
}

CMEncoderHandle CMEncoderH264 = {
	.init = init_yuv_h264,
	.set_format = enc_h264_set_format,
	.set_size = set_solution,
	.start = open_yuv_h264,
	.stop = close_yuv_h264,
	.push = encoder_yuv_h264,
	.pop = enc_h264_pop,
	.deinit = enc_h264_deinit,
};


