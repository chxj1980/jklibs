/*********************************************************
 *
 * Filename: demo/demo_unix_server.c
 *   Author: jmdvirus
 *   Create: 2018年09月07日 星期五 12时01分58秒
 *
 *********************************************************/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "cm_unixsocket.h"

int main() {
	unlink("/tmp/demo_ux");
	CMUnixSocketServerHandle h;
	int ret = cm_unixsocket_server_init(&h, "/tmp/demo_ux");
	if (ret < 0) {
		printf("Error init server socket [%s]\n", strerror(errno));
		return -1;
	}
	//cm_unixsocket_server_nonblock(h, 0);
	while (1) {
		printf("Start accept\n");
	    int client = cm_unixsocket_server_accept(h);
	    if (client > 0) {
	        cm_unixsocket_server_nonblock(h, client);
	    	char data[1024] = {0};
	    	int lendata = 1024;
	    	while (1) {
	    		int ret = cm_unixsocket_server_recv(h, client, data, &lendata);
				if (ret < 0) {
				    if (errno == EAGAIN || errno == EWOULDBLOCK) {
				    	usleep(50000);
				    	continue;
				    }
					printf("recv error [%d][%s]\n", errno, strerror(errno));
					break;
				}
	    		printf("recv out [%d][%s]\n", lendata, data);
	    		usleep(800000);
	    		char senddata[1024] = {0};
	    		sprintf(senddata, "recv len %d resp", lendata);
	    		int lensend = strlen(senddata);
				printf("Start to send data out [%d]\n", lensend);
	    		ret = cm_unixsocket_server_send(h, client, senddata, lensend);
				if (ret < 0) {
					printf("send error [%d][%s]\n", errno, strerror(errno));
					break;
				} else {
					printf("send success [%d]\n", ret);
				}
	    	}
	    }
		usleep(50000);
		//sleep(1);
	}
	cm_unixsocket_server_deinit(&h);

	return 0;
}

