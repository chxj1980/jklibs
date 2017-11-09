/*
 * Author: jmdvirus
 * Create: 2017-11-09
 */

#include <malloc.h>
#include <sys/ioctl.h>

#include "vdev.h"
#include "base.h"

/*
 * Open device
 * return: < 0 fail, 0 success
 */
API_PREFIX int VDevOpen(VDevHandle *handle, const char *devname) {
    VDev *dev = (VDev*)malloc(sizeof(VDev));
    sprintf(dev->devname, "%s", devname);
    int ret = open_device(dev);
    if (ret < 0) {
        free(dev);
        return -1;
    }
    *handle = dev;
    return 0;
}

/*
 * Free
 */
API_PREFIX int VDevClose(VDevHandle *handle) {
    VDev *dev = *handle;
    close_device(dev);
    free(dev);
    *handle = NULL;
    return 0;
}

/*
 * Start Stream
 */
API_PREFIX int VDevStart(VDevHandle handle) {
    VDev *dev = (VDev*)handle;
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    return ioctl (dev->devhandle, VIDIOC_STREAMON, &type);
}

/*
 * Stop stream
 */
API_PREFIX int VDevStop(VDevHandle handle) {
    VDev *dev = (VDev*)handle;
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    return ioctl (dev->devhandle, VIDIOC_STREAMOFF, &type);
}

/*
 * Get Video data
 * return:
 * 0 success
 * < 0 fail
 */
API_PREFIX int VDevGetStream(VDevHandle handle, char **buffer, unsigned int *len) {
    VDev *dev = (VDev*)handle;
    int ret = get_video_data(dev, buffer, len);
    return ret;
}
