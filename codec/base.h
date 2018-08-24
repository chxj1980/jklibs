/**
 * Create: 2017-11-10
 * Author: jmdvirus
 */
#ifndef __CODEC_BASE_H
#define __CODEC_BASE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "codec.h"
#include "x264.h"

typedef struct {
	void          *szPrivate;
} CMEncoder;

typedef struct {
	int (*init)(CMEncoder *enc);
	int (*set_format)(CMEncoder *enc, int format);
	int (*set_size)(CMEncoder *enc, int w, int h);
	int (*start)(CMEncoder *enc);
	int (*stop)(CMEncoder *enc);

    // push one frame to encode
	int (*push)(CMEncoder *enc, unsigned char *data, unsigned int len);

	// Get pointer of the data, you must push out,
	// It will free when call stop and deinit
    // return 0: success, data len will put in *len
	int (*pop)(CMEncoder *enc, unsigned char **data, unsigned int *len);
	int (*deinit)(CMEncoder *enc);
} CMEncoderHandle;

extern CMEncoderHandle CMEncoderH264;
extern CMEncoderHandle CMEncoderH265;

#ifdef __cplusplus
}
#endif

#endif

