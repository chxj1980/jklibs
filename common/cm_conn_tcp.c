/*
 *===========================================================================
 *
 *          Name: cm_conn_tcp.c
 *        Create: 2015年09月25日 星期五 19时58分05秒
 *
 *   Discription: 
 *
 *        Author: jmdvirus
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
#include <netdb.h>

#include "cm_conn_tcp.h"
#include "cm_utils.h"
#include "cm_print.h"

struct tagCMConnTCP {
    char        addr[64];
    int         port;
    int         sockFD;
    struct sockaddr_in   addr_in;
    
    int         status;
};

int cm_conn_tcp_create(CMConnTCP *conn, const char *addr, int port)
{
    if (*conn != NULL || !addr || port <= 0 || port > 65535) return -1;

    CMConnTCP inConn = (CMConnTCP)cm_mem_malloc(sizeof(struct tagCMConnTCP));
    if (!inConn) return -2;

    sprintf(inConn->addr, "%s", addr);
    inConn->port = port;

    inConn->sockFD = socket(AF_INET, SOCK_STREAM, 0);
    if (!inConn->sockFD) {
        return -3;
    }

    struct hostent *host = gethostbyname(addr);
    if (!host) {
        return -4;
    }

    bzero(&inConn->addr_in, sizeof(struct sockaddr_in));
    inConn->addr_in.sin_family = AF_INET;
    //inet_pton(AF_INET, inConn->addr, &inConn->addr_in.sin_addr);
    // sprintf((char*)&inConn->addr_in.sin_addr, "%s", host->h_addr_list[0]);
    inConn->addr_in.sin_addr.s_addr = inet_addr(addr);
    cmdebug("Server ip is: %s\n", inet_ntoa(*(struct in_addr*)host->h_addr_list[0]));

    inConn->addr_in.sin_port = htons(inConn->port);
    
    if (conn) *conn = inConn;
    return 0;
}

int cm_conn_tcp_connect(CMConnTCP conn, int bCycle, int waitTime, int checktimes)
{
    if (!conn) return -1;
    if (waitTime < 0) waitTime = 1;

    int ct = -1;
    if (checktimes > 0) ct = checktimes;
    int ctnow = 0;
    do {
        if (ct != -1) {
            if (ctnow++ > ct) break;
        }
        int ret = connect(conn->sockFD, (struct sockaddr *)&conn->addr_in, 
                        sizeof(struct sockaddr));
        if (ret == -1) {
            if (errno == EISCONN) {
                conn->status = 1;
                break;
            }
            cmdebug("connect falied, reconnect ... %x,%s\n", errno, strerror(errno));
            sleep(waitTime);
            continue;
        }
        conn->status = 1;
        break;
    } while(bCycle);

    return conn->status;
}

int cm_conn_tcp_close(CMConnTCP *conn)
{
    if (!conn) return -1;
    if ((*conn)->sockFD) close((*conn)->sockFD);
    free(*conn);
    *conn = NULL;
    return 0;
}

int cm_conn_tcp_send(CMConnTCP conn, const char *data, long len)
{
    if (!conn || !data || len <= 0) return -111;

    // TODO: use cycle to send more data
    long remain = len;
    long sendlen = 0;
    int sendval = 2048;
    do {
        sendval = remain < sendval ? remain : sendval;
        int out = send(conn->sockFD, data+sendlen, sendval, MSG_NOSIGNAL);
        if (out < 0) return out;
        sendlen += out;
        remain -= out;
    } while(remain > 0);
    return sendlen;
}

int cm_conn_tcp_recv(CMConnTCP conn, char *recvData, long *recvLen, int bCycle)
{
    if (!conn || !recvData || !recvLen) return -1;

    int sockfd = conn->sockFD;
    long containLen = *recvLen;
    *recvLen = 0;
    int checktimes = 50;
    int recv_counts = 0;
    do {
        char data[1024] = {0};
        int lendata = 1024;
        int nBytes = recv(sockfd, data, lendata, MSG_DONTWAIT);
        if (nBytes == 0) {
            if (recv_counts++ > checktimes) break;
            continue;
        }
        if (nBytes == -1) {
            if (recv_counts++ > checktimes) break;
            if (errno == EAGAIN || errno == EWOULDBLOCK) continue;
            cmerror("Read data error %s\n", strerror(errno));
            conn->status = 0;
            return -2;
        }
        memcpy(recvData + *recvLen, data, nBytes);
        *recvLen += nBytes;

        if (*recvLen >= containLen) {
            return 1;
        }
    } while(bCycle);

    return 0;
}

/*=============== End of file: cm_conn_tcp.c ==========================*/
