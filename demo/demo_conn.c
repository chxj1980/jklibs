/*
 *===========================================================================
 *
 *          Name: demo_conn.c
 *        Create: 2015年09月28日 星期一 09时03分42秒
 *
 *   Discription: 
 *
 *        Author: jmdvirus
 *         Email: jmdvirus@roamter.com
 *
 *===========================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "jk_conn_tcp.h"
#include "rt_print.h"

int main(int argc, char **args)
{
    JKConnTCP ct = NULL;
    int ret = jk_conn_tcp_create(&ct, args[1], atoi(args[2]));
    if (ret != 0) {
        rterror("conn handle create failed. %d", ret);
        return -1;
    }

    rtinfo("start connect ...");
    ret = jk_conn_tcp_connect(ct, 1, 2, 5);
    if (ret < 0) {
        rterror("connect failed %d", ret);
        return -2;
    }

    rtinfo("start send data");
    char *thisdata = (char*)"I'll send data";
    ret = jk_conn_tcp_send(ct, thisdata, strlen(thisdata));
    if (ret < 0) {
        rterror("send failed %d", ret);
        return -3;
    }

    char recvData[1024] = {0};
    long recvLen = 1024;
    rtinfo("start recv data...");
    ret = jk_conn_tcp_recv(ct, recvData, &recvLen, 0);
    if (ret < 0) {
        rterror("recv data failed. %d", ret);
        return -4;
    }
    if (recvLen > 0) {
        rtinfo("Recv data : %d, %s", recvLen, recvData);
    }

    rtinfo("close...");
    ret = jk_conn_tcp_close(&ct);
    if (ret < 0) {
        rterror("closed connect %d", ret);
        return -5;
    }

    int cnts = 0;
    while (1) {
        if (cnts++ > 20) break;
        sleep(1);
    };

    rtinfo("Closed all.");

    return 0;
}

/*=============== End of file: demo_conn.c ==========================*/
