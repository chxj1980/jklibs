//
// Created by v on 17-6-26.
//

#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "rt_print.h"
#include "unixsocket/rt_unixsocket.h"
#include <sys/time.h>

typedef struct {
    unsigned int utc;
    unsigned int timestamp;
    unsigned char key_flag;
    unsigned char rsv[3];
} MediaDataHead;

long long get_time_ms()
{
#ifndef _WIN32
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec*1000 + tv.tv_usec / 1000;
#else
    return 0;
#endif
}

typedef struct _NaluUnit
{
    int type;
    int size;
    int startcode_size;
    unsigned char *data;
} NaluUnitVA;

int ReadNaluFromBufVA( NaluUnitVA *nalu, char* buf, int len, int nalhead_pos) {
    nalu->size = 0;
    int naltail_pos = nalhead_pos;
    while (nalhead_pos < len) {
        /* search for nal header */
        if (buf[nalhead_pos++] == 0x00 &&
            buf[nalhead_pos++] == 0x00) {
            if (buf[nalhead_pos++] == 0x01) {
                nalu->startcode_size = 3;
                goto gotnal_head;
            } else {
                /* cuz we have done an i++ before,so we need to roll back now */
                nalhead_pos--;
                if (buf[nalhead_pos++] == 0x00 &&
                    buf[nalhead_pos++] == 0x01) {
                    nalu->startcode_size = 4;
                    goto gotnal_head;
                } else
                    continue;
            }
        } else
            continue;

        /* search for nal tail which is also the head of next nal */
        gotnal_head:
        /* normal case:the whole nal is in this m_pFileBuf */
        naltail_pos = nalhead_pos;
        while (naltail_pos < len) {
            if (buf[naltail_pos++] == 0x00 &&
                buf[naltail_pos++] == 0x00) {
                if (buf[naltail_pos++] == 0x01) {
                    nalu->size = (naltail_pos - 3) - nalhead_pos;
                    break;
                } else {
                    naltail_pos--;
                    if (buf[naltail_pos++] == 0x00 &&
                        buf[naltail_pos++] == 0x01) {
                        nalu->size = (naltail_pos - 4) - nalhead_pos;
                        break;
                    }
                }
            }
        }

        nalu->type = buf[nalhead_pos] & 0x1f;
//        if (nalu->startcode_size == 4) {
//            nalu->startcode_size = 3;
//        }
        nalu->data = (unsigned char *) buf + nalhead_pos - nalu->startcode_size;
        if (nalu->size == 0) {
            // can not find last
            nalu->size = naltail_pos - nalhead_pos + nalu->startcode_size;
            return naltail_pos;
        } else {
            nalu->size += nalu->startcode_size;
        }
        nalhead_pos = naltail_pos;
        return nalhead_pos - 4;
    }
    return -1;
}

int do_write_data(const char *unixpath, const char *videopath) {
    RTUnixSocketClientHandle h;
    int ret = rt_unixsocket_client_init(&h, unixpath);
    if (ret != 0) {
        printf("ERROR: unix init failed %d, %s\n", ret, strerror(errno));
        return -1;
    }

    int len = 5*1024*1024;
    char *data = (char*)malloc(len);

    int start_timestamp = get_time_ms();

    static FILE *file = fopen(videopath, "rb");
    if (!file) {
        printf("ERROR: video path open failed\n");
        return -4;
    }

    ret = fread(data, 1, len, file);
    if (ret < 0) {
        printf("ERROR: read data failed %d\n", ret);
        return -5;
    }

    printf("INFO: read out data len %d\n", len);
    int nalu_pos = 0;
    int counts = 0;
    int sleep_time = 40000;
    for (;;) {
        NaluUnitVA nu;
        memset(&nu, 0, sizeof(NaluUnitVA));
        nalu_pos = ReadNaluFromBufVA(&nu, (char *) data, ret, nalu_pos);
        if (nalu_pos < 0) {
            printf("INFO: read data done\n");
            break;
        }

        if (counts++ == 200 || counts == 400) {
            sleep_time = 70000;
            start_timestamp += 40;
            printf("INFO: lost one %d\n", counts);
            continue;
        }

        int buf_len = nu.size + 40;
        unsigned char *buf = (unsigned char*)malloc(buf_len);
        MediaDataHead md;
        memset(&md, 0, sizeof(MediaDataHead));
        md.timestamp = start_timestamp;
        memcpy(buf, &md, sizeof(MediaDataHead));
        memcpy(buf+40, nu.data, nu.size);

        printf("INFO: nalu_post %d timestamp %lu send data to unix socket %d\n", nalu_pos, md.timestamp, buf_len);
        int ret1 = rt_unixsocket_client_send(h, (char*)buf, buf_len);
        if (ret1 < 0) {
            printf("ERROR: send data failed %d\n", ret1);
            return -2;
        }

        start_timestamp += 40;
        printf("OK: send done\n");
        usleep(sleep_time);
        sleep_time = 40000;
    }

    free(data);
    return 0;
}

int main(int argc, char **args) {
    char *unixpath= args[1];
    char *videopath = args[2];
    do_write_data(unixpath, videopath);

    while(1) {
        usleep(40000);
    }

    return 0;
}