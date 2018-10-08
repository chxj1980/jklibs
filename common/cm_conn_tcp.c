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
#include <fcntl.h>
#ifdef _MAXOS
#include <mach/error.h>
#else
#include <error.h>
#endif
#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>

#include "cm_conn_tcp.h"
#include "cm_utils.h"

/*
 * About getaddrinfo, we use thread to connect, and we shouldn't cancel the thread as the memory will not free.
 * So we let the thread exit by itself, but parent thread has exit.
 * Here, you known, if getaddrinfo thread has too many, the memory will be used so much, we need give count
 * for how many threads have, and wait if threads greater then some value(50).
 */
static int tcp_addrinfo_count = 0;

struct tagCMConnTCP {
    char        addr[64];
    int         port;
    int         sockFD;
    struct sockaddr_in   addr_in;
    
    int         status;
    int         iTimeout;
    int         iMainTimeout;
    int         iSuccess; // for getaddrinfo
    pthread_t         iMainThread;
};

int cm_conn_tcp_create(CMConnTCP *conn, const char *addr, int port)
{
    if (*conn != NULL || !addr || port <= 0 || port > 65535) return -1;

    CMConnTCP inConn = (CMConnTCP)cm_mem_malloc(sizeof(struct tagCMConnTCP));
    if (!inConn) return -2;

    sprintf(inConn->addr, "%s", addr);
    inConn->port = port;

    inConn->sockFD = socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, 0);
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

    inConn->addr_in.sin_port = htons(inConn->port);
    
    if (conn) *conn = inConn;
    return 0;
}

#define TIME_DEBUG

void* cm_conn_tcp_create_thread(void *args)
{
    CMConnTCP oconn = (CMConnTCP)args;
    struct tagCMConnTCP conn;
    memcpy(&conn, oconn, sizeof(struct tagCMConnTCP));

    printf("-------------------------- thread created\n");
    char strport[16] = {0};
    sprintf(strport, "%d", conn.port);
    struct addrinfo hints;
    struct addrinfo *results = NULL, *rp = NULL;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;

#ifdef TIME_DEBUG
    unsigned long long t1 = cm_gettime_milli();
#endif
    int n = getaddrinfo(conn.addr, strport, &hints, &results);
    if (n != 0) {
        printf("get addrinfo failed [%d] errno [%d, %s]\n", n, errno, strerror(errno));
        if (results) {
            freeaddrinfo(results);
        }
        tcp_addrinfo_count --;
        return NULL;
    }
#ifdef TIME_DEBUG
    unsigned long long t2 = cm_gettime_milli();
    printf("get addrinfo use time [%llu] ms\n", t2 - t1);
#endif

    for (rp = results; rp != NULL; rp = rp->ai_next) {
        conn.sockFD = socket(rp->ai_family, rp->ai_socktype,
                                rp->ai_protocol);
        if (conn.sockFD == -1)
            continue;
//        fcntl(inConn->sockFD, F_SETFL, O_NONBLOCK);
        if (connect(conn.sockFD, rp->ai_addr, rp->ai_addrlen) != -1)
            break;
        close(conn.sockFD);
    }
    freeaddrinfo(results);
#ifdef TIME_DEBUG
    unsigned long long t3 = cm_gettime_milli();
    printf("try connect use time [%llu] ms\n", t3 - t2);
#endif
    if (rp == NULL) {
        printf("get addrinfo connect failed\n");
        tcp_addrinfo_count--;
        return NULL;
    }
    int ret = pthread_kill(conn.iMainThread, 0);
    if (ret == ESRCH) {
    } else {
        oconn->iSuccess = 1;
        oconn->sockFD = conn.sockFD;
        printf("thread getaddrinfo success\n");
    }
    tcp_addrinfo_count--;
    printf("---------------------- thread done\n");
    return NULL;
}

int cm_conn_tcp_create_new(CMConnTCP *conn, const char *addr, int port)
{
    if (*conn != NULL || !addr || port <= 0 || port > 65535) return -1;

    CMConnTCP inConn = (CMConnTCP)cm_mem_malloc(sizeof(struct tagCMConnTCP));
    if (!inConn) return -2;

    sprintf(inConn->addr, "%s", addr);
    inConn->port = port;
    char strport[16] = {0};
    sprintf(strport, "%d", inConn->port);
    struct addrinfo hints;
    struct addrinfo *results, *rp;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;

    int n = getaddrinfo(inConn->addr, strport, &hints, &results);
    if (n != 0) {
//        cmerror("Error get addrinfo [%d] [%s]\n", n, gai_strerror(n));
        cm_mem_free(inConn);
        return -3;
    }

    for (rp = results; rp != NULL; rp = rp->ai_next) {
        inConn->sockFD = socket(rp->ai_family, rp->ai_socktype,
                              rp->ai_protocol);
        if (inConn->sockFD == -1)
            continue;
//        fcntl(inConn->sockFD, F_SETFL, O_NONBLOCK);
        if (connect(inConn->sockFD, rp->ai_addr, rp->ai_addrlen) != -1)
            break;
        close(inConn->sockFD);
    }
    freeaddrinfo(results);
    if (rp == NULL) {
//        cmerror("Failed connect to any address\n");
        cm_mem_free(inConn);
        return -4;
    }

    if (conn) *conn = inConn;
    return 0;
}

void *cm_conn_tcp_create_thread_wait(void *args)
{
    CMConnTCP conn = (CMConnTCP)args;

    pthread_t ithread = 0;
    int ret = pthread_create(&ithread, NULL, cm_conn_tcp_create_thread, conn);
    if (ret < 0) {
        cm_mem_free(conn);
        return NULL;
    }
    tcp_addrinfo_count ++;
    pthread_detach(ithread);
    printf("[%p] thread wait for timeout [%d]\n", conn, conn->iTimeout);

    int count = 0;
    int btimeout = 1;
    while (count++ < 20 * conn->iTimeout) {
        ret = pthread_kill(ithread, 0);
        if (ret == 0) {
            usleep(50000);
        } else if (ret == ESRCH) {
            btimeout = 0;
            break;
        } else {
            usleep(50000);
        }
    }
    if (btimeout) {
        conn->iMainTimeout = 1;
    }
    printf("[%p] thread wait exit with timeout [%d]\n", conn, btimeout);
    return NULL;
}

int cm_conn_tcp_create_new_timeout(CMConnTCP *conn, const char *addr, int port, int timeout)
{
    if (*conn != NULL || !addr || port <= 0 || port > 65535) return -1;
    if (tcp_addrinfo_count > 50) return -23;

    CMConnTCP inConn = (CMConnTCP)cm_mem_malloc(sizeof(struct tagCMConnTCP));
    if (!inConn) return -2;
    inConn->iSuccess = 0;
    inConn->iTimeout = timeout;
    inConn->iMainTimeout = 0;
    printf("[%p] create wait thread timeout original [%d] thread count[%d]\n", inConn, inConn->iTimeout, tcp_addrinfo_count);

    sprintf(inConn->addr, "%s", addr);
    inConn->port = port;

    int ret = pthread_create(&inConn->iMainThread, NULL, cm_conn_tcp_create_thread_wait, inConn);
    if (ret < 0) {
        cm_mem_free(inConn);
        return -3;
    }
//    pthread_detach(inConn->iMainThread);
    pthread_join(inConn->iMainThread, NULL);
    printf("[%p] create thread exit timeout [%d], success [%d] \n", inConn, inConn->iMainTimeout, inConn->iSuccess);
    if (inConn->iMainTimeout == 1) {
        cm_mem_free(inConn);
        return -4;
    }
    if (!inConn->iSuccess) {
        cm_mem_free(inConn);
        return -5;
    }

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
    conn->status = 0;
    do {
        if (ct != -1) {
            if (ctnow++ >= ct) break;
        }
        int ret = connect(conn->sockFD, (struct sockaddr *)&conn->addr_in, 
                        sizeof(struct sockaddr));
        if (ret == -1) {
            if (errno == EISCONN) {
                conn->status = 1;
                break;
            }
//            cmdebug("connect falied, reconnect ... %x,%s\n", errno, strerror(errno));
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
    int sendval = 20480;
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
        char data[10240] = {0};
        int lendata = 10240;
        int nBytes = recv(sockfd, data, lendata, MSG_DONTWAIT);
        if (nBytes == 0) {
            if (recv_counts++ > checktimes) break;
            continue;
        }
        if (nBytes == -1) {
            if (recv_counts++ > checktimes) break;
            if (errno == EAGAIN || errno == EWOULDBLOCK) continue;
//            cmerror("Read data error %s\n", strerror(errno));
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

int cm_conn_tcp_recv_simple(CMConnTCP conn, char *recvData, int *recvlen)
{
    if (!conn || !recvData) return -12;

    int sockfd = conn->sockFD;
    int n = 0;
    fd_set fdread;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 400000;
    do {
        FD_ZERO(&fdread);
        FD_SET(sockfd, &fdread);
        int ret = select(sockfd+1, &fdread, NULL, NULL, &tv);
        if (ret == -1) {
            return -1;
        } else if (ret == 0) {
            return 0;
        }
    } while(0);

    do {
        if (!FD_ISSET(sockfd, &fdread)) {
            return 0;
        }
        n = recv(sockfd, recvData, *recvlen, MSG_DONTWAIT);
        if (n == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                usleep(10000);
                continue;
            }
        }

        break;
    } while(1);

    // If select tell me have data
    // but recv give no data
    // we think connect is disconnected.
    if (n == 0) return -1;
    if (n > 0) *recvlen = n;

    return n;
}

/*=============== End of file: cm_conn_tcp.c ==========================*/
