/*
 * Author: jmdvirus
 * Create: 2017-11-09
 */

#include "base.h"
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>

int open_device(VDev *dev) {
    dev->devhandle = open(dev->devname, O_RDWR|O_NONBLOCK);
    if (dev->devhandle < 0) {
        return -1;
    }

    get_capability(dev);
    get_fmtdesc(dev);

    int ret = set_fmt(dev, 0);
    if ( ret < 0) {
        return ret;
    }
    ret = request_buffers(dev);
    if (ret < 0) {
        return ret;
    }

    return 0;
}

int get_capability(VDev *dev) {
    return ioctl(dev->devhandle, VIDIOC_QUERYCAP, &dev->cap);
}

int get_fmtdesc(VDev *dev) {
    int i = -1;
    do {
        i++;
        dev->desc[i].index = i;
        dev->desc[i].type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    } while(ioctl(dev->devhandle, VIDIOC_ENUM_FMT, &dev->desc[i]) != -1);
    dev->desc_count = i + 1;
    return i + 1;
}

int close_device(VDev *dev) {
    if (dev->devhandle > 0) {
        close(dev->devhandle);
    }
    return 0;
}

int set_fmt(VDev *dev, int fmt) {
    struct v4l2_format format;
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    //format.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB32;
    format.fmt.pix.width = 640;
    format.fmt.pix.height = 480;
    format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    format.fmt.pix.field = V4L2_FIELD_INTERLACED;
    int ret = ioctl(dev->devhandle, VIDIOC_S_FMT, &format);
    if (ret < 0) {
        return -1;
    }
    return 0;
}

int request_buffers(VDev *dev) {
    dev->req.count = 8;
    dev->req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    dev->req.memory = V4L2_MEMORY_MMAP;
    int ret = ioctl(dev->devhandle, VIDIOC_REQBUFS, &dev->req);
    if (ret < 0) {
        return -1;
    }
    dev->buffers = (VBuffers*)calloc(dev->req.count, sizeof(VBuffers));
    if (!dev->buffers) {
        return -2;
    }
    // mmap
    for (unsigned int i = 0; i < dev->req.count; ++i) {
         struct v4l2_buffer buf;
         memset(&buf, 0, sizeof(buf));
         buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
         buf.memory = V4L2_MEMORY_MMAP;
         buf.index = i;
         ret = ioctl(dev->devhandle, VIDIOC_QUERYBUF, &buf);
         if (ret < 0) {
             free(dev->buffers);
             return -3;
         }
         dev->buffers[i].length = buf.length;
         dev->buffers[i].start = mmap (NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, dev->devhandle, buf.m.offset);
         if (dev->buffers[i].start == MAP_FAILED) {
             free(dev->buffers);
             return -4;
         }
         ret = ioctl(dev->devhandle, VIDIOC_QBUF, &buf);
         if (ret < 0) {
             free(dev->buffers);
             return -5;
         }
    }
    return 0;
}

int get_video_data(VDev *dev, char **buffer, unsigned int *length) {
    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(struct v4l2_buffer));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    // 从缓冲区取出一个缓冲帧
    ioctl(dev->devhandle, VIDIOC_DQBUF, &buf);
    // 图像处理
    // process_image(buffers[buf.index].start);
    *length = dev->buffers[buf.index].length;
    *buffer = dev->buffers[buf.index].start;
    // 将取出的缓冲帧放回缓冲区
    ioctl(dev->devhandle, VIDIOC_QBUF,&buf);
    return 0;
}

int debug_capability(VDev *dev) {
    printf("Driver caps: \n"
           "Driver : %s\n"
           "Card : %s\n"
           "Bus : %s\n"
           "Version: %d \n"
           "Capabilities : %x\n",
           dev->cap.driver, dev->cap.card, dev->cap.bus_info, dev->cap.version, dev->cap.capabilities
    );
    return 0;
}

int debug_fmtdesc(VDev *dev) {
    printf("Driver fmt desc count [%d]\n", dev->desc_count);
    for (int i = 0; i < dev->desc_count; i++) {
         printf("index: %d, desc: %s\n", dev->desc[i].index, dev->desc[i].description);
    }
    return 0;
}

