
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "codec.h"
#include "base.h"

int init_yuv_h264(CEncoder *en, int yuv_type) {
    memset(en, 0, sizeof(CEncoder));
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
	if (yuv_type == CODEC_VIDEO_YUV422) {
        en->codec_type = X264_CSP_I422;
	} else if (yuv_type == CODEC_VIDEO_YUV420) {
		en->codec_type = X264_CSP_I420;
	}
    x264_param_apply_profile(&en->param, x264_profile_names[4]);

    return 0;
}

int set_solution(CEncoder *en, int h, int w) {
    en->param.i_width = w;
    en->param.i_height = h;
    return 0;
}

int open_yuv_h264(CEncoder *en) {
    x264_picture_init(&en->pic_out);
	int i_csp = 0;
    x264_picture_alloc(&en->pic_in, en->codec_type, en->param.i_width, en->param.i_height);
    en->pic_in.img.i_csp = en->codec_type;
    en->pic_in.img.i_plane = 3;
    en->pic_in.i_pts = 1;
    int msize = en->param.i_width * en->param.i_height * 2;
    en->outdata = (char*)malloc(msize);
    en->outlength = 0;

    en->handle = x264_encoder_open(&en->param);
    return en->handle == NULL ? -1 : 0;
}

int close_yuv_h264(CEncoder *en) {
    x264_picture_clean(&en->pic_in);
    x264_encoder_close(en->handle);
    return 0;
}

static int encoder_yuv422_h264(CEncoder *en, char *data, unsigned int length) {
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

static int encoder_yuv420_h264(CEncoder *en, char *data, unsigned int length) {
    int   iNals = 0;
	en->pic_in.img.plane[0] = (uint8_t*)data;
	en->pic_in.img.plane[1] = (uint8_t*)data + en->param.i_height;
	en->pic_in.img.plane[2] = (uint8_t*)data + en->param.i_height*5/4;
    en->pic_in.i_pts ++;
    en->outlength = 0;
    int frame_size = x264_encoder_encode(en->handle, &en->nal, &iNals, &en->pic_in, &en->pic_out);
	printf("-------- %d\n", frame_size);

    if (frame_size < 0) return -1;
	return iNals;
}

int encoder_yuv_h264(CEncoder *en, char *data, unsigned int length) {
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
