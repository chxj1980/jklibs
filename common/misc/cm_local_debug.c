/*********************************************************
 *
 * Filename: base/cm_local_debug.cpp
 *   Author: jmdvirus
 *   Create: 2018年12月03日 星期一 14时35分05秒
 *
 *********************************************************/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "cm_local_debug.h"
#include "cm_conn_udp.h"
#include "cm_logprint.h"

void *local_debug_thread(void *args)
{
	CMLocalDebug *ld = (CMLocalDebug*)args;
	cminfo("local debug thread start\n");

	char data[10240] = {0};
	long len = 10240;
	int ret = 0;
	while (!ld->iExit) {

		ret = cm_conn_udp_recv(ld->pHD, data, &len, 0, 0);
		if (ret >= 0 && len > 0) {
			cmdebug("local debug recv data len[%d] [%s]\n", len, data);
			if (ld->cbRecvData) {
				ld->cbRecvData(data, len, ld->pUserdata);
			}
		}

		usleep(50000);
	}
	sem_post(&ld->iSem);

	cminfo("local debug thread end\n");
	return NULL;
}

int cm_local_debug_init(CMLocalDebug *ld, int port, on_recvdata recvdata, void *userdata)
{
	if (!ld) return -1;

	memset(ld, 0, sizeof(CMLocalDebug));

	int ret = cm_conn_udp_create(&ld->pHD, "0.0.0.0", port);
	if (ret < 0) {
		cmerror("local debug udp create failed [%d]\n", ret);
		return -2;
	}

	cm_conn_udp_bind(ld->pHD);

	ret = pthread_create(&ld->iThread, NULL, local_debug_thread, ld);
	if (ret < 0) {
		cmerror("local debug pthread create failed [%d]\n", ret);
		return -3;
	}
	pthread_detach(ld->iThread);
	cmdebug("local debug init success\n");

    ld->cbRecvData = recvdata;
	ld->pUserdata = userdata;
	sem_init(&ld->iSem, 0, 0);

	return 0;
}

int cm_local_debug_deinit(CMLocalDebug *ld)
{
	if (!ld) return -1;
	ld->iExit = 1;
	sem_wait(&ld->iSem);
	cm_conn_udp_close(&ld->pHD);

	cmdebug("local debug deinit success\n");
	sem_destroy(&ld->iSem);

	return 0;
}

int cm_local_debug_send(CMLocalDebug *ld, const char *data, int len)
{
	if (!ld) return -1;
	int ret = -2;
	if (ld->pHD) {
		ret = cm_conn_udp_send(ld->pHD, data, len);
	}
	return ret;
}

