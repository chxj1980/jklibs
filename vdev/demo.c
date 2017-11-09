/*
 * Author: jmdvirus
 * Create: 2017-11-09
 */

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "vdev.h"
#include "base.h"

int main(int argc, char **args) {
    VDevHandle dev;
    int ret = VDevOpen(&dev, args[1]);
    if (ret < 0) {
        printf("Error: open device [%s] , %d\n", args[1], ret);
        return -1;
    }

    debug_capability(dev);
    debug_fmtdesc(dev);

    ret = set_fmt(dev, 0);
    if (ret < 0) {
        printf("Set format failed\n");
    } else {
        printf("Set format success\n");
    }

    ret = VDevStart(dev);
    if (ret < 0) {
        printf("Dev start error %d\n", ret);
    } else {
        printf("Dev start success\n");
    }

    FILE *file = fopen("/tmp/video1.x", "w");
    char *data = NULL;
    unsigned int len = 0;
    time_t last = time(NULL);
    while(1) {
        if (time(NULL) - last > 5) break;
        VDevGetStream(dev, &data, &len);
        printf("Get  Stream len %u, data %p\n", len, data);

        fwrite(data, 1, len, file);

        usleep(40000);
    }
    fclose(file);

    VDevStop(dev);
    VDevClose(&dev);

    return 0;
}
