/*********************************************************
 *
 * Filename: demo/demo_debug_out.c
 *   Author: jmdvirus
 *   Create: 2018年12月12日 星期三 11时19分51秒
 *
 *********************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cm_logprint.h"
#include "cm_debug_out.h"
#include "cm_sys.h"

CMLogPrint logPrint;

static CMDebugOut dout;

typedef struct {
	int        x;
	int        iExit;
} CMDebugDemo;

static CMDebugDemo dd;

int recv_data(int type, char *data, size_t length, void *userdata)
{
	cmdebug("recv data type [%d], length [%u] userdata [%p], data \n%s\n", 
			type, length, userdata, data);

	CMDebugDemo *ind = (CMDebugDemo*)userdata;
	cmdebug("demo value x=%d\n", ind->x);

	char sdata[] = "{\"type\":\"response\", \"value\":\"xyz\"}";
	size_t slen = strlen(sdata);
	char rdata[1024] = {0};
	size_t rlen = sizeof(rdata);
	cm_debug_out_request(dout, 1, sdata, slen, rdata, &rlen);
	if (rlen > 0) {
		cmdebug("request response len [%d] data \n%s\n", rlen, rdata);
	}
	cm_sys_usleep(1000000); // wait send done.
	ind->iExit = 1;
	return 0;
}

int main(int argc, char *args[]) {
	if (argc < 3) {
		return 0;
	}

	CM_LOG_PRINT_INIT("debugout");

	dd.x = 8;

	char *addr = args[1];
	int port = atoi(args[2]);
    char *url = "/message/ws";

	cmdebug("start\n");
	int ret = 0;
	ret = cm_debug_out_init(&dout, url, addr, port, &recv_data, &dd);
	if (ret < 0) {
	    cmerror("debug out init result [%d]\n", ret);
		dd.iExit = 1;
	} else {
		cminfo("debug out init success\n");
		dd.iExit = 0;
	}

	while (!dd.iExit) {
		cm_debug_out_run(dout);
		cm_sys_usleep(50000);
	}

	cminfo("debut out start to deinit\n");
	cm_debug_out_deinit(&dout);

	cmdebug("exit\n");
	getchar();

	return 0;
}

