/**
 * Author: jmdvirus
 * Create: 2017-11-10
 */
#ifndef __CODEC_CODEC_H
#define __CODEC_CODEC_H

#define API_PREFIX
#ifdef _WIN32
#endif

#ifdef __cplusplus
extern "C" {
#endif

enum {
    CODEC_VIDEO_UYVY = 1,
    CODEC_VIDEO_YUV420 = 2,
    CODEC_VIDEO_YUV422 = 3,
    CODEC_VIDEO_YUV444 = 4,
    CODEC_VIDEO_RGB32 = 5,
    CODEC_VIDEO_H264 = 6,
    CODEC_VIDEO_H265 = 7,
};

typedef void *CodecHandle;

API_PREFIX int CodecInit(CodecHandle *handle, int input, int output);

API_PREFIX int CodecSetSolution(CodecHandle handle, int w, int h);

API_PREFIX int CodecOpen(CodecHandle handle);

API_PREFIX int CodecClose(CodecHandle *handle);

API_PREFIX int CodecPush(CodecHandle handle, unsigned char *data, unsigned int length);

API_PREFIX int CodecPop(CodecHandle handle, unsigned char **data, unsigned int *length);

#ifdef __cplusplus
}
#endif

#endif

