//
// Created by v on 16-7-11.
//

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "usock.h"

int main() {
    printf("Start to use usock\n");

    int ret = usock(USOCK_TCP|USOCK_IPV4ONLY, "192.168.134.56", 0);
    printf("Create uscok ret: %d\n", ret);
    if (ret < 0) {
        printf("Error: %d, %s\n", errno, strerror(errno));
    }
    close(ret);
    ret = 0;

    ret = usock(USOCK_UNIX, "/tmp/usock.sock", 0);
    printf("Create unix socket ret: %d\n", ret);
    if (ret < 0) {
        printf("Error: %d, %s\n", errno, strerror(errno));
    }
    close(ret);

    return 0;
}