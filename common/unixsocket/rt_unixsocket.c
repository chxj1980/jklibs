//
// Created by v on 16-3-2.
// Author: jmdvirus@roamter.com
//

#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "rt_unixsocket.h"
#include "bvpu_utils.h"
#include "rt_print.h"
#include "jk_common.h"

#define RT_UNIXSOCKET_PATH_MAX  256

struct tagRTUnixSocketServerHandle {
    char          szUnixPath[RT_UNIXSOCKET_PATH_MAX];  // socket path
    struct sockaddr_un  szAddr;
    int           iFD;
};

int rt_unixsocket_server_init(RTUnixSocketServerHandle *h, const char *path)
{
    if (!path || !h) return -1;
    if (strlen(path) >= RT_UNIXSOCKET_PATH_MAX) {
        return -2;
    }

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (!fd) {
        return -3;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path)-1);

    unlink(path);

    int ret = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
    if (ret == -1) {
        rterror("bind failed: %d,%s", ret, strerror(ret));
        return -4;
    }

    ret = listen(fd, 2);
    if (ret == -1) {
        rterror("listen failed: %d, %s", ret, strerror(ret));
        return -5;
    }

    RTUnixSocketServerHandle inH = (RTUnixSocketServerHandle)jk_mem_calloc(1, sizeof(struct tagRTUnixSocketServerHandle));
    if (!inH) {
        close(fd);
        return JK_RESULT_E_MALLOC_FAIL;
    }

    strncpy(inH->szUnixPath, path, RT_UNIXSOCKET_PATH_MAX-1);
    inH->iFD = fd;
    memcpy(&inH->szAddr, &addr, sizeof(addr));

    *h = inH;

    return 0;
}

int rt_unixsocket_server_deinit(RTUnixSocketServerHandle *h)
{
    if (*h) {
        close((*h)->iFD);
        bvpu_mem_free(*h);
    }
    *h = NULL;
    return 0;
}

int rt_unixsocket_server_client_close(int client)
{
    if (client) {
        close(client);
    }
    return 0;
}

int rt_unixsocket_server_accept(RTUnixSocketServerHandle h)
{
    int client = 0;
    client = accept(h->iFD, NULL, NULL);
    if (client == -1) {
        rterror("accept failed: %d", client);
    }
    return client;
}

int rt_unixsocket_server_recv(RTUnixSocketServerHandle h, int client, char *data, int *len)
{
    if (!h || !client || !data || !len) return -1;

    int rc = 0;
    rc = recv(client, data, *len, 0);
    if (rc == -1) {
        rterror("read failed: %d,%s", rc, strerror(rc));
        return -2;
    } else if (rc == 0) {

    }
    *len = rc;
    return *len;
}

int rt_unixsocket_server_send(RTUnixSocketServerHandle h, int client, char *data, int len)
{
    if (!h || !client || !data || len <= 0) return -1;

    int rc = send(client, data, len, 0);
    if (rc == -1) {
        rterror("send failed: %d, %s", rc, strerror(rc));
        return -2;
    }
    return rc;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// Client from here
////////////////////////////////////////////////////////////////////////////////////////////

struct tagRTUnixSocketClientHandle {
    char          szUnixPath[RT_UNIXSOCKET_PATH_MAX];  // socket path
    struct sockaddr_un  szAddr;
    int           iFD;
    int           iConn;  // if has connected.
};

int rt_unixsocket_client_init(RTUnixSocketClientHandle *h, const char *path)
{
    if (!path || !h) return -1;
    if (strlen(path) >= RT_UNIXSOCKET_PATH_MAX) {
        return -2;
    }

    if (access(path, F_OK) != 0) {
        return -5;
    }

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (!fd) {
        return -3;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path)-1);

    RTUnixSocketClientHandle inH = (RTUnixSocketClientHandle)jk_mem_calloc(1, sizeof(struct tagRTUnixSocketServerHandle));
    if (!inH) {
        close(fd);
        return JK_RESULT_E_MALLOC_FAIL;
    }

    strncpy(inH->szUnixPath, path, RT_UNIXSOCKET_PATH_MAX);
    inH->iFD = fd;
    memcpy(&inH->szAddr, &addr, sizeof(addr));
    inH->iConn = 0;

    *h = inH;

    return 0;
}

int rt_unixsocket_client_deinit(RTUnixSocketClientHandle *h)
{
    if (*h) {
        close((*h)->iFD);
        bvpu_mem_free(*h);
        *h = NULL;
    }
    return 0;
}

int rt_unixsocket_client_send(RTUnixSocketClientHandle h, char *data, int len)
{
    if (!h) return -1;

    if (!h->iConn) {
        int c = connect(h->iFD, (struct sockaddr *) &h->szAddr, sizeof(h->szAddr));
        if (c == -1) {
            rterror("connect failed: %d,%s", c, strerror(errno));
            return -2;
        } else {
            rtinfo("connect [%s] success\n", h->szUnixPath);
        }
        h->iConn = 1;
    }

    int sendcnts = 0;
    if (data && len > 0) {
        int n = write(h->iFD, data, len);
        if (n <= 0 || n != len) {
            rterror("send failed: %d", n);
            if (n == -1) {
                h->iConn = 0; // maybe connect failed.
                rterror("send failed: %s", strerror(errno));
                return -3;
            }
        }
        sendcnts = n;
    }

    return sendcnts;
}

int rt_unixsocket_client_recv(RTUnixSocketClientHandle h, char *data, int *len)
{
    if (!h || !data || !len) return -1;

    if (!h->iConn) return -2;

    int c = recv(h->iFD, data, *len, 0);
    if (c == -1) {
        rterror("read failed: %d,%s", c, strerror(c));
        return -3;
    }
    *len = c;

    return 0;
}
