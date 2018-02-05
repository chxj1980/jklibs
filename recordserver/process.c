/*
 */

#include "process.h"

int rs_init(GlobalInfo *gi)
{
    int ret = VDevOpen(&gi->devHandle, gi->videoDev);
    if (ret < 0) {
        rterror("Error open [%s] %d\n", gi->videoDev, ret);
        return -1;
    }

    ret = CodecInit(&gi->codecHandle, CODEC_VIDEO_YUV422, CODEC_VIDEO_H264);
    if (ret < 0) {
        rterror("Error init codec [%d]\n", ret);
        return -3;
    }

    return 0;
}

int rs_deinit(GlobalInfo *gi)
{
    CodecClose(&gi->codecHandle);

    VDevStop(gi->devHandle);
    VDevClose(&gi->devHandle);
    return 0;
}

int rs_run(GlobalInfo *gi)
{
    VDevGetStream(gi->devHandle, &gi->stream_data, &gi->stream_len);
    rtdebug("Get stream len %u, yuv real len %u\n", gi->stream_len, gi->stream_yuvlen);
    int ret = CodecPush(gi->codecHandle, gi->stream_data, gi->stream_yuvlen);
    if (ret < 0) {
        rterror("Codec push data failed %d\n", ret);
        return -1;
    }
    gi->stream_delen = 0;
    ret = CodecPop(gi->codecHandle, &gi->stream_dedata, &gi->stream_delen);
    if (ret < 0) {
        rterror("Codec pop data failed %d\n", ret);
        return -2;
    }
    return gi->stream_delen;
}

int rs_start(GlobalInfo *gi, int width, int height)
{
    CodecSetSolution(gi->codecHandle, width, height);
    gi->stream_yuvlen = width * height * 2;
    int ret = CodecOpen(gi->codecHandle);
    if (ret < 0) {
        rterror("Error open open codec %d\n", ret);
        return -4;
    }
   
    ret = VDevStart(gi->devHandle);
    if (ret < 0) {
        rterror("Error start stream %d\n", ret);
        return -2;
    } else {
        rtinfo("Success start stream\n");
    }

    return 0;
}

int sock_init(GlobalInfo *gi, const char *unixpath)
{
    int ret = rt_unixsocket_client_init(&gi->sock, unixpath);
    if (ret < 0) {
        rterror("unix socket [%s] init failed [%d]\n", unixpath, ret);
        return -1;
    }
    return 0;
}

int sock_send(GlobalInfo *gi, unsigned char *data, unsigned int len)
{
    return rt_unixsocket_client_send(gi->sock, data, len);
}

int sock_deinit(GlobalInfo *gi)
{
    rt_unixsocket_client_deinit(&gi->sock);
    return 0;
}

