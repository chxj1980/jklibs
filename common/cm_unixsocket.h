//
// Created by v on 16-3-2.
//

#ifndef CMPROGRAM_CM_UNIXSOCKET_H_H
#define CMPROGRAM_CM_UNIXSOCKET_H_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagCMUnixSocketServerHandle *CMUnixSocketServerHandle;
typedef struct tagCMUnixSocketClientHandle *CMUnixSocketClientHandle;

int cm_unixsocket_server_init(CMUnixSocketServerHandle *h, const char *path);

int cm_unixsocket_server_deinit(CMUnixSocketServerHandle *h);

int cm_unixsocket_server_client_close(int client);

int cm_unixsocket_server_accept(CMUnixSocketServerHandle h);

int cm_unixsocket_server_recv(CMUnixSocketServerHandle h, int client, char *data, int *len);

int cm_unixsocket_server_send(CMUnixSocketServerHandle h, int client, char *data, int len);

/////////////////////////////////////////////////////////////////////
// client
int cm_unixsocket_client_init(CMUnixSocketClientHandle *h, const char *path);

int cm_unixsocket_client_deinit(CMUnixSocketClientHandle *h);

int cm_unixsocket_client_send(CMUnixSocketClientHandle h, char *data, int len);

int cm_unixsocket_client_recv(CMUnixSocketClientHandle h, char *data, int *len);

#ifdef __cplusplus
}
#endif

#endif //CMPROGRAM_CM_UNIXSOCKET_H_H
