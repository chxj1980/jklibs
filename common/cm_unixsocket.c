//
// Created by v on 16-3-2.
//

#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include "cm_unixsocket.h"
#include "cm_utils.h"
#include "cm_common.h"

#define CM_UNIXSOCKET_PATH_MAX  256

struct tagCMUnixSocketServerHandle {
    char          szUnixPath[CM_UNIXSOCKET_PATH_MAX];  // socket path
    struct sockaddr_un  szAddr;
    int           iFD;
};

int cm_unixsocket_server_init(CMUnixSocketServerHandle *h, const char *path)
{
    if (!path || !h) return -1;
    if (strlen(path) >= CM_UNIXSOCKET_PATH_MAX) {
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
//        cmerror("bind failed: %d,%s", ret, strerror(ret));
        return -4;
    }

    ret = listen(fd, 2);
    if (ret == -1) {
//        cmerror("listen failed: %d, %s", ret, strerror(ret));
        return -5;
    }

    CMUnixSocketServerHandle inH = (CMUnixSocketServerHandle)cm_mem_calloc(1, sizeof(struct tagCMUnixSocketServerHandle));
    if (!inH) {
        close(fd);
        return -6;
    }

    strncpy(inH->szUnixPath, path, CM_UNIXSOCKET_PATH_MAX-1);
    inH->iFD = fd;
    memcpy(&inH->szAddr, &addr, sizeof(addr));

    *h = inH;

    return 0;
}

int cm_unixsocket_server_deinit(CMUnixSocketServerHandle *h)
{
    if (*h) {
        close((*h)->iFD);
        cm_mem_free(*h);
    }
    *h = NULL;
    return 0;
}

int cm_unixsocket_server_nonblock(CMUnixSocketServerHandle h, int client)
{
	if (!h) return -1;
	if (client > 0) fcntl(client, F_SETFL, SOCK_NONBLOCK);
	else fcntl(h->iFD, F_SETFL, SOCK_NONBLOCK);
	return 0;
}

int cm_unixsocket_server_fcntl(CMUnixSocketServerHandle h, int flag)
{
	if (!h) return -1;
	return fcntl(h->iFD, F_SETFL, flag);
}

int cm_unixsocket_server_client_close(int client)
{
    if (client) {
        close(client);
    }
    return 0;
}

int cm_unixsocket_server_accept(CMUnixSocketServerHandle h)
{
    int client = 0;
    client = accept(h->iFD, NULL, NULL);
    if (client == -1) {
//        cmerror("accept failed: %d", client);
    }
    return client;
}

int cm_unixsocket_server_recv(CMUnixSocketServerHandle h, int client, char *data, int *len)
{
    if (!h || !client || !data || !len) return -1;

    int rc = -1;
    rc = recv(client, data, *len, 0);
	if (rc >= 0) *len = rc;
    return rc == -1 ? -1 : 0;
}

int cm_unixsocket_server_send(CMUnixSocketServerHandle h, int client, char *data, int len)
{
    if (!h || !client || !data || len <= 0) return -1;

    int rc = send(client, data, len, MSG_NOSIGNAL);
    if (rc == -1) {
//        cmerror("send failed: %d, %s", rc, strerror(rc));
        return -2;
    }
    return rc;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// Client from here
////////////////////////////////////////////////////////////////////////////////////////////

struct tagCMUnixSocketClientHandle {
    char          szUnixPath[CM_UNIXSOCKET_PATH_MAX];  // socket path
    struct sockaddr_un  szAddr;
    int           iFD;
    int           iConn;  // if has connected.
};

int cm_unixsocket_client_init(CMUnixSocketClientHandle *h, const char *path)
{
    if (!path || !h) return -1;
    if (strlen(path) >= CM_UNIXSOCKET_PATH_MAX) {
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

    CMUnixSocketClientHandle inH = (CMUnixSocketClientHandle)cm_mem_calloc(1, sizeof(struct tagCMUnixSocketServerHandle));
    if (!inH) {
        close(fd);
        return -6;
    }

    strncpy(inH->szUnixPath, path, CM_UNIXSOCKET_PATH_MAX);
    inH->iFD = fd;
    memcpy(&inH->szAddr, &addr, sizeof(addr));
    inH->iConn = 0;

    *h = inH;

    return 0;
}

int cm_unixsocket_client_deinit(CMUnixSocketClientHandle *h)
{
    if (*h) {
        close((*h)->iFD);
        cm_mem_free(*h);
        *h = NULL;
    }
    return 0;
}

int cm_unixsocket_client_nonblock(CMUnixSocketClientHandle h)
{
	if (!h) return -1;
	return fcntl(h->iFD, F_SETFL, SOCK_NONBLOCK);
}

int cm_unixsocket_client_fcntl(CMUnixSocketClientHandle h, int flag)
{
	if (!h) return -1;
	return fcntl(h->iFD, F_SETFL, flag);
}

int cm_unixsocket_client_send(CMUnixSocketClientHandle h, char *data, int len)
{
    if (!h) return -1;

    if (!h->iConn) {
        int c = connect(h->iFD, (struct sockaddr *) &h->szAddr, sizeof(h->szAddr));
        if (c == -1) {
			printf("connect failed : %d, %s\n", errno, strerror(errno));
//            cmerror("connect failed: %d,%s", c, strerror(errno));
            return -2;
        } else {
			printf("connect success\n");
//            cminfo("connect [%s] success\n", h->szUnixPath);
        }
        h->iConn = 1;
    }

    int sendcnts = 0;
    if (data && len > 0) {
        int n = send(h->iFD, data, len, MSG_NOSIGNAL);
		if (n == -1) {
			h->iConn = 0;
			return n;
		}
		sendcnts = n;
    }

    return sendcnts;
}

int cm_unixsocket_client_recv(CMUnixSocketClientHandle h, char *data, int *len)
{
    if (!h || !data || !len) return -1;

    if (!h->iConn) return -2;

    int c = recv(h->iFD, data, *len, 0);
    if (c >= 0) *len = c;

    return c == -1 ? -3 : 0;
}
