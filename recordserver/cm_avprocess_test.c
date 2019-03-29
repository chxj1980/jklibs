//
// Created by v on 19-2-22.
//

#include <stdio.h>
#include <unistd.h>
#include <malloc.h>

#include "cm_avprocess.h"
#include "cm_logprint.h"

CMLogPrint logPrint;

void avprocess_test() {
    CMAVProcessP  av;
    cm_avprocess_init(&av);

    cm_avprocess_av_ready(av, 640, 480);

    FILE *f = fopen("/opt/data/output/1-yuv420.yuv", "r");
    int len = 640 * 480 * 3/2;
    unsigned char *data = (unsigned char*)malloc(len);
    if (f) {
        int frames = 0;
        while (1) {
            int ret = fread(data, 1, len, f);
            if (ret <= 0) {
                if (ret == 0) {
                    cminfo("read done\n");
                } else {
                    cmerror("read error %d\n", ret);
                }
                break;
            }
            cm_avprocess_write(av, data, len);
            cminfo("process write frames %d\n", frames++);
        }

        fclose(f);
    } else {
        cmerror("file open failed\n");
    }

    cm_avprocess_deinit(&av);
}

int main(int argc, char **args) {
    CM_LOG_PRINT_INIT("avprocess");
    avprocess_test();
    return 0;
}