/*********************************************************
 *
 * Filename: base/cm_debug_out.c
 *   Author: jmdvirus
 *   Create: 2018年12月12日 星期三 10时42分40秒
 *
 *********************************************************/

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "cm_debug_out.h"
#include "cm_conn_tcp.h"
#include "cm_conn_ws.h"
#include "cm_logprint.h"

struct _CMDebugOut {
	char         szURL[256];
	char         szAddr[64];
	int          iPort;
	recv_callback   cb;
    CMConnTCP    conn;
	int          iWSOK;

	char         *data;
	size_t       length;

	void         *priv;
};

int cm_debug_out_init(CMDebugOut *dout, const char *url, const char *addr, int port, recv_callback cb, void *userdata)
{
	CMDebugOut in = (CMDebugOut)calloc(1, sizeof(struct _CMDebugOut));
	if (!in) return -1;

	strncpy(in->szURL, url, sizeof(in->szURL) -1);
	strncpy(in->szAddr, addr, sizeof(in->szAddr) -1);
	in->iPort = port;
	in->cb = cb;
	in->priv = userdata;

	cm_conn_tcp_create_new(&in->conn, addr, port);

	if (dout) *dout = in;
	return 0;
}

int cm_debug_out_deinit(CMDebugOut *dout)
{
	if (!dout) return 0;
	CMDebugOut in = *dout;
	if (in) {
		cmdebug("start to free [%p] conn [%p] data [%p]\n", in, in->conn, in->data);
        if (in->conn) {
			cm_conn_tcp_close(&in->conn);
		}
		if (in->data) {
			free(in->data);
		}
		free(in);
	}
	*dout = NULL;
	return 0;
}

int cm_debug_out_request(CMDebugOut dout, int waitres, const char *data, size_t length,
		char *resdata, size_t *reslength)
{
	if (!dout) return -1;

	int ret = cm_websocket_request(dout->conn, waitres, data, length, resdata, reslength);
	return ret;
}

int cm_debug_out_run(CMDebugOut dout)
{
	if (!dout) return -1;
	int ret = 0;

	if (cm_conn_tcp_status(dout->conn) <= 0) {
		dout->iWSOK = 0;
		if (dout->conn) cm_conn_tcp_close(&dout->conn);
	    cm_conn_tcp_create_new(&dout->conn, dout->szAddr, dout->iPort);
		if (cm_conn_tcp_status(dout->conn) <= 0) {
			return -2;
		} else {
			cmdebug("debug out reconnect success\n");
		}
	}

	if (!dout->iWSOK) {
		ret = cm_websocket_handshake(dout->conn, dout->szURL, dout->szAddr, dout->iPort);
		if (ret >= 0) {
			cmdebug("debug out handle shake success\n");
			dout->iWSOK = 1;
		} else {
			cmdebug("debug out handle shake fail [%d]\n", ret);
			return -3;
		}
	}

	ret = cm_websocket_request_recv_ex(dout->conn, &dout->data, &dout->length);
	if (ret > 0 && dout->cb) {
		dout->cb(CM_DOUT_DATA, dout->data, dout->length, dout->priv);
	}

	return 0;
}

