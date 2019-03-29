//
// Created by v on 19-2-22.
//

#include <stdio.h>

#include "cm_avprocess.h"
#include "codec.h"
#include "avstorage.h"
#include "cm_logprint.h"
#include "cm_utils.h"

struct tagCMAVProcess {
    CodecHandle  pHandle;
    AVStorageP   pStorage;
};

int cm_avprocess_init(CMAVProcessP *av)
{
    CMAVProcessP in = (CMAVProcessP) cm_mem_malloc(sizeof(struct tagCMAVProcess));
    if (!in) return -1;

    int ret = CodecInit(&in->pHandle, CODEC_VIDEO_YUV420, CODEC_VIDEO_H265);
    if (ret < 0) {
        cmerror("avprocess codec init failed ret [%d]\n", ret);
        cm_mem_free(in);
        return -2;
    }
    ret = cm_avstorage_init(&in->pStorage, "/opt/data/data/out", 1800, 30*1024*1024);
    if (ret < 0) {
        cmerror("avprocess storage init fail ret [%d]\n", ret);
        CodecClose(&in->pHandle);
        cm_mem_free(in);
    }

    cmdebug("avprocess init done\n");
    *av = in;
    return 0;
}

int cm_avprocess_deinit(CMAVProcessP *av)
{
    if (!av) return -1;
    CMAVProcessP in = *av;
    if (in) {
        cm_avstorage_deinit(&in->pStorage);
        CodecClose(&in->pHandle);
        cm_mem_free(in);
    }
    *av = NULL;
    cmdebug("avprocess deinit done\n");
    return 0;
}

int cm_avprocess_av_ready(CMAVProcessP av, int width, int height)
{
    if (!av) return -1;
    if (!av->pHandle) return -2;
    CodecSetSolution(av->pHandle, width, height);
    int ret = CodecOpen(av->pHandle);
    if (ret != 0) {
        cmerror("avprocess codec open fail [%d]\n", ret);
        return -3;
    }
    cmdebug("avprocess ready [%d, %d]\n", width, height);
    return 0;
}

int cm_avprocess_write(CMAVProcessP av, unsigned char *data, size_t len)
{
    if (!av) return -1;
    if (!av->pHandle || !av->pStorage) return -2;

    CodecPush(av->pHandle, data, len);

    unsigned char *outdata = NULL;
    unsigned int outlen = 0;
    int ret = CodecPop(av->pHandle, &outdata, &outlen);
    if (ret < 0) {
        cmerror("avprocess codec decode error\n");
        return -3;
    }

    cm_avstorage_v_push(av->pStorage, outdata, outlen, 0);

    return 0;
}
