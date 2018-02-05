
/*
 *
 */

#include <stdio.h>
#include <unistd.h>

#include "vdev.h"
#include "rt_print.h"
#include "codec.h"
#include "rt_unixsocket.h"

typedef struct _GlobalInfo {
    VDevHandle   devHandle;
    CodecHandle  codecHandle;
    RTUnixSocketClientHandle   sock;
    char         videoDev[32];
    FILE         *fs;
    char         *stream_data;
    unsigned int  stream_len;
    char         *stream_dedata;
    unsigned int  stream_delen;
    unsigned int  stream_yuvlen;
} GlobalInfo;

extern GlobalInfo gi;

