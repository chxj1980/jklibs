/*********************************************************
 *
 * Filename: codec/cmyuv.h
 *   Author: jmdvirus
 *   Create: 2019年01月23日 星期三 13时57分09秒
 *
 *********************************************************/

#ifndef __CODEC_YUV_H_
#define __CODEC_YUV_H_

#ifdef __cplusplus
extern "C" {
#endif

int cm_yuy2_yuv420p(const char *data, int width, int height, char *outdata);

#ifdef __cplusplus
}
#endif

#endif

