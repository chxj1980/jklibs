//
// Created by v on 16-1-4.
//

/*
 *===========================================================================
 *
 *          Name: cm_conn_udp.c
 *        Create: 2016年01月4日 星期五 19时58分05秒
 *
 *   Discription: 
 *
 *        Author: jmdvirus
 *         Email: jmdvirus@aliyun.com
 *
 *===========================================================================
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>   
#include <sys/socket.h>   
#include <arpa/inet.h>   
#include <netinet/in.h>   
#include <stdlib.h>
#ifdef _MAXOS
#include <mach/error.h>
#else
#include <error.h>
#endif
#include <errno.h>

#include "cm_conn_udp.h"
#include "cm_utils.h"

struct tagCMConnUDP {
    char        addr[64];
    int         port;
    int         sockFD;
    struct sockaddr_in   addr_in;

    int         status;
};

int cm_conn_udp_create(CMConnUDP *conn, const char *addr, int port)
{
    if (*conn != NULL || !addr || port <= 0 || port > 65535) return -1;

    CMConnUDP inConn = (CMConnUDP)cm_mem_malloc(sizeof(struct tagCMConnUDP));
    if (!inConn) return -2;

    sprintf(inConn->addr, "%s", addr);
    inConn->port = port;

    inConn->sockFD = socket(AF_INET, SOCK_DGRAM, 0);
    if (!inConn->sockFD) {
        return -3;
    }

    bzero(&inConn->addr_in, sizeof(struct sockaddr_in));
    inConn->addr_in.sin_family = AF_INET;
    inet_pton(AF_INET, inConn->addr, &inConn->addr_in.sin_addr);
    inConn->addr_in.sin_port = htons(inConn->port);

    if (conn) *conn = inConn;
    return 0;
}

int cm_conn_udp_bind(CMConnUDP conn)
{
	if (!conn) return -1;
	bind(conn->sockFD, (struct sockaddr*)&conn->addr_in, sizeof(conn->addr_in));

	return 0;
}

int cm_conn_udp_connect(CMConnUDP conn, int bCycle, int waitTime, int checktimes)
{
    if (!conn) return -1;
    if (waitTime < 0) waitTime = 1;

    int ct = 0;
    if (checktimes > 0) ct = checktimes;
    int ctnow = 0;
    do {
        if (ctnow++ > ct) break;
        int ret = connect(conn->sockFD, (struct sockaddr *)&conn->addr_in,
                          sizeof(struct sockaddr));
        if (ret == -1) {
            //rterror("connect falied, reconnect ... %s", strerror(errno));
            sleep(waitTime);
            continue;
        }
        conn->status = 1;
        break;
    } while(bCycle);

    return conn->status;
}

int cm_conn_udp_close(CMConnUDP *conn)
{
    if (!conn) return -1;
    if ((*conn)->sockFD) close((*conn)->sockFD);
    free(*conn);
    *conn = NULL;
    return 0;
}

int cm_conn_udp_send(CMConnUDP conn, const char *data, long len)
{
    if (!conn || !data || len <= 0) return -1;

    // TODO: use cycle to send more data
    long remain = len;
    long sendlen = 0;
    int sendval = 1024;
    do {
        sendval = remain < sendval ? remain : sendval;
        int out = sendto(conn->sockFD, data+sendlen, sendval, 0, (struct sockaddr*)&conn->addr_in, sizeof(struct sockaddr));
        if (out < 0) return errno;
        sendlen += out;
        remain -= out;
    } while(remain > 0);
    return sendlen;
}

int cm_conn_udp_recv(CMConnUDP conn, char *recvData, long *recvLen, int bCycle, int block)
{
    if (!conn || !recvData || !recvLen) return -1;

    int sockfd = conn->sockFD;
    long containLen = *recvLen;
    *recvLen = 0;
    int dontwait = block == 0 ? MSG_DONTWAIT : 0;
	int have_data = 0;
	size_t lensock = sizeof(struct sockaddr);
    do {
        char data[10240] = {0};
        int lendata = 10240;
        int nBytes = recvfrom(sockfd, data, lendata, dontwait, 
					(struct sockaddr*)&conn->addr_in, &lensock);
        if (nBytes == 0 && have_data == 0) {
			if (bCycle) continue;
			else break;
		}
        if (nBytes == -1) {
            if (errno == EAGAIN || errno == EINTR) {
                continue;
            }
            //rterror("Read data error fd[%d] %s", sockfd, strerror(errno));
            conn->status = 0;
            return -2;
        }
		have_data = 1;
        memcpy(recvData + *recvLen, data, nBytes);
        *recvLen += nBytes;

        if (*recvLen >= containLen) {
            return 1;
        }
        if (!bCycle) break;
    } while(1);

    return 0;
}

/*=============== End of file: cm_conn_tcp.c ==========================*/
