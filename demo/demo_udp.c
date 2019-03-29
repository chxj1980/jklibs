//
// Created by v on 16-1-4.
// Author: jmdvirus@roamter.com
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "jk_conn_udp.h"
#include "rt_print.h"

int main(int args, char **argv) {
    if (args < 3) {
        rterror("%s [ipaddr] [port]", argv[0]);
        return -1;
    }

    char *ip = argv[1];
    char *portstr = argv[2];

    int port = atoi(portstr);

    rtinfo("connect to ip %s:%d", ip, port);

    JKConnUDP udphd = NULL;
    int ret = jk_conn_udp_create(&udphd, ip, port);
    if (ret < 0) {
        rterror("conn udp create failed: %d", ret);
        return -2;
    }

    ret = jk_conn_udp_connect(udphd, 1, 2, 5);
    if (ret < 0) {
        rterror("conn udp connect failed %d", ret);
        return -3;
    }

    char *sendData = "I was c implement.";
    size_t lendata = strlen(sendData);
    ret = jk_conn_udp_send(udphd, sendData, lendata);
    if (ret < 0) {
        rterror("send data failed %d", ret);
        return -4;
    }
    rtinfo("Send data out of len %d", ret);

    char recvData[1024] = {0};
    size_t lenRecv = 1024;
    ret = jk_conn_udp_recv(udphd, recvData, (long *) &lenRecv, 0, 0);
    if (ret < 0) {
        rterror("recv data failed %d", ret);
        return -5;
    }

    rtinfo("recv data len: %d, %s", lenRecv, recvData);

    jk_conn_udp_close(&udphd);

    return 0;
}