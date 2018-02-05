/*
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include "process.h"

GlobalInfo gi;
static int quit = 1;

int main(int argc, char **args) 
{
    if (argc <= 1) {
        fprintf(stderr, "Use -h to see help\n");
        return -1;
    }
    char *dev = NULL;
    char *path = NULL;
    int debug = 0;
    int opt = 0;
    while((opt = getopt(argc, args, "i:p:dh")) != -1) {
        switch(opt) {
            case 'i':
                dev = optarg;
                break;
            case 'p':
                path = optarg;
                break;
            case 'd':
                debug = atoi(optarg);
                break;
            case 'h':
            default:
                fprintf(stderr, "Usage: %s -i video_dev -p path -d debug\n", args[0]);
                return -3;
        }
    }

    snprintf(gi.videoDev, sizeof(gi.videoDev), "%s", dev);

    if (debug) {
        gi.fs = fopen("/tmp/v.h264", "w");
        if (!gi.fs) {
            rterror("Error open file to write\n");
            return -5;
        }
    }

    int ret = rs_init(&gi);
    if (ret < 0) {
        rterror("rs init failed [%d]\n", ret);
        return -1;
    }

    ret = rs_start(&gi, 640, 480);
    if (ret < 0) {
        rterror("rs start failed [%d]\n", ret);
        return -2;
    }
    ret = sock_init(&gi, "/tmp/av.unix");
    if (ret < 0) {
        rterror("sock init failed [%d]\n", ret);
    }

    time_t last = time(NULL);
    quit = 0;
    while (1) {
        if (quit && time(NULL) - last > 15) break;
        ret = rs_run(&gi);
        rtdebug("Codec Decoder len %d\n", gi.stream_delen);
        if (gi.stream_delen > 0) {
            ret = sock_send(&gi, gi.stream_dedata, gi.stream_delen);
            rtdebug("sock send data ret [%d] len [%d]\n", ret, gi.stream_delen);
        }

        if (debug) {
            if (gi.stream_delen > 0) {
                fwrite(gi.stream_dedata, 1, gi.stream_delen, gi.fs);
            }
        }

        usleep(40000);
    }

    if (debug) {
        if (gi.fs) fclose(gi.fs);
    }

    sock_deinit(&gi);
    rs_deinit(&gi);

    return 0;
}
