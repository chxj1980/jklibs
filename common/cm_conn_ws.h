//
// Created by v on 18-6-5.
//

#ifndef SRC_CM_CONN_WS_H
#define SRC_CM_CONN_WS_H

#include <stdio.h>

#include "cm_conn_tcp.h"

#ifdef __cplusplus
extern "C" {
#endif

int cm_websocket_request_send_wait_res(CMConnTCP conn, const char *msg, size_t len, char *res, size_t *lenres);

/**
 * Websocket recv, this will recv and parse, until all data recv done. and put realdata (exclude header) to @result
 *
 * */
int cm_websocket_request_recv(CMConnTCP conn, char *result, size_t *lenresult);

int cm_websocket_request_recv_ex(CMConnTCP conn, char **result, size_t *lenresult);

/*
 * Request with websocket
 */
int cm_websocket_request(CMConnTCP conn, int waitres, const char *msg, size_t len, char *res, size_t *lenres);

/**
 * Handshake of websocket
 * @url: like /chat, /message/ws
 * return: 0 - success, other - fail
 * */
int cm_websocket_handshake(CMConnTCP conn, const char *url, const char *addr, int port);


#ifdef __cplusplus
}
#endif

#endif //SRC_CM_CONN_WS_H
