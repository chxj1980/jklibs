//
// Created by v on 16-3-2.
// Author: jmdvirus@roamter.com
//

#ifndef RTPROGRAM_RT_UNIXSOCKET_H_H
#define RTPROGRAM_RT_UNIXSOCKET_H_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagRTUnixSocketServerHandle *RTUnixSocketServerHandle;
typedef struct tagRTUnixSocketClientHandle *RTUnixSocketClientHandle;

int rt_unixsocket_server_init(RTUnixSocketServerHandle *h, const char *path);

int rt_unixsocket_server_deinit(RTUnixSocketServerHandle *h);

int rt_unixsocket_server_client_close(int client);

int rt_unixsocket_server_accept(RTUnixSocketServerHandle h);

int rt_unixsocket_server_recv(RTUnixSocketServerHandle h, int client, char *data, int *len);

int rt_unixsocket_server_send(RTUnixSocketServerHandle h, int client, char *data, int len);

/////////////////////////////////////////////////////////////////////
// client
int rt_unixsocket_client_init(RTUnixSocketClientHandle *h, const char *path);

int rt_unixsocket_client_deinit(RTUnixSocketClientHandle *h);

int rt_unixsocket_client_send(RTUnixSocketClientHandle h, char *data, int len);

int rt_unixsocket_client_recv(RTUnixSocketClientHandle h, char *data, int *len);

#ifdef __cplusplus
}
#endif

#endif //RTPROGRAM_RT_UNIXSOCKET_H_H
