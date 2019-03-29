//
// Created by v on 16-4-12.
// Author: jmdvirus@roamter.com
//


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "jk_conn_tcp.h"
#include "rt_print.h"

static JKConnTCP inConn;
static int hasConnected = 0;

int connping(const char *addr, int port) {
    int ret = jk_conn_tcp_create(&inConn, addr, port);
    if (ret < 0) {
        return -1;
    }
    ret = jk_conn_tcp_connect(inConn, 1, 5, 1);
    if (ret < 0) {
        jk_conn_tcp_close(&inConn);
        return -2;
    }
    ret = jk_conn_tcp_send(inConn, "YE", 2);
    if (ret != 2) {
        hasConnected = 0;
        jk_conn_tcp_close(&inConn);
        return -3;
    }
    char data[16] = {0};
    long lenrecv = 0;
    ret = jk_conn_tcp_recv(inConn, data, &lenrecv, 0);
    if (ret <= 0) {
        jk_conn_tcp_close(&inConn);
        return -4;
    }
    jk_conn_tcp_close(&inConn);

    return 0;
}

int main(int argc, char **args) {
    if (argc < 3) {
        rterror("failed args counts ");
        return -1;
    }
    char *addr = args[1];
    int port = atoi(args[2]);
    rtdebug("addr %s, port %d", addr, port);
//    while (1) {
        int ret = connping(addr, port);
        rtinfo("result of conn ping %d", ret);
//        sleep(10);
//    }
    return ret;
}
