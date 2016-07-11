//
// Created by v on 16-3-3.
// Author: jmdvirus@roamter.com
//

#include <stdio.h>
#include "zlog.h"

int main(int argc, char **args) {
    printf("Start test zlog.");

    int rc = dzlog_init("/etc/zlog.conf", "zlog_demo");
    if (rc) {
        printf("init error %d\n", rc);
        return -1;
    }

    dzlog_info("a test ");

    zlog_fini();
    return 0;
}
