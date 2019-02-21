/*********************************************************
 *
 * Filename: cmbusserver.c
 *   Author: jmdvirus
 *   Create: 2019年01月07日 星期一 15时40分10秒
 *
 *********************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "busserver_ex.h"
#include "cm_logprint.h"

static CMBusserverP gs;
CMLogPrint logPrint;

static int doid = 0;

void server_recv(const char *topic, const char *topic_data, int data_len, void *user)
{
	cmdebug("busserver recv [%p] topic [%s] topic data [%d][%s]\n", user, topic, data_len, topic_data);
	switch (doid) {
		case 0:
			cm_busserver_send_msg(gs, "demo1", "/mm/m1", "nothing");
			break;
		case 1:
			cm_busserver_send_msg(gs, "demo2", "/mm/m2", "stop");
			break;
		case 2:
			cm_busserver_broadcast(gs, "/every/one", "welcome");
			break;
		default:
			break;
	}
	doid++;
}

int main(int argc, char *args[]) {
	CM_LOG_PRINT_INIT("busserver");

	int port = 50001;
	if (argc > 1) {
		port = atoi(args[1]);
	}
	int ret = cm_busserver_init(&gs, gs, port, &server_recv);
	if (ret < 0) {
		cmerror("busserver init failed\n");
		return -1;
	}

	cminfo("busserver start to run on [%d]\n", port);
	cm_busserver_run(gs);
	cminfo("busserver exit\n");
	cm_busserver_deinit(&gs);
	return 0;
}

