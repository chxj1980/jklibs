/*
 * Author: jmdviurs
 * Create: 2017-11-09
 */

#include <linux/videodev2.h>

typedef struct {
    void *start;
    unsigned int length;
} VBuffers, *VBuffersP;

typedef struct {
    char    devname[256];
    int     devhandle;
    struct  v4l2_capability cap;
    struct  v4l2_fmtdesc desc[16];
    int     desc_count;
    struct  v4l2_requestbuffers req;
    VBuffers  *buffers;
} VDev, *VDevP;

int open_device(VDev *dev);

int close_device(VDev *dev);

int get_capability(VDev *dev);

int get_fmtdesc(VDev *dev);

int set_fmt(VDev *dev, int fmt);

int request_buffers(VDev *dev);

int get_video_data(VDev *dev, char **buffer, unsigned int *length);

int debug_capability(VDev *dev);

int debug_fmtdesc(VDev *dev);
