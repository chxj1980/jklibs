/*********************************************************
 *
 * Filename: demo/demo_unix_client.c
 *   Author: jmdvirus
 *   Create: 2018年09月07日 星期五 12时21分07秒
 *
 *********************************************************/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "cm_unixsocket.h"

int main() {
    CMUnixSocketClientHandle h;
	while (1) {
	    int ret = cm_unixsocket_client_init(&h, "/tmp/demo_ux");
	    if (ret < 0) {
            printf("client init fail, wait\n");
			sleep(1);
			continue;
	    }
		cm_unixsocket_client_nonblock(h);
		//sleep(5);

		while(1) {
			char data[1024] = "hello socket";
			int lendata = strlen(data);
			printf("start to send data\n");
			int ret = cm_unixsocket_client_send(h, data, lendata);
			if (ret < 0) {
				printf("send error [%d][%s]\n", errno, strerror(errno));
				sleep(1);
				continue;
			}
			char sdata[1024] = {0};
			int lens = 1024;
			printf("start to recv data\n");
			while (1) {
			    ret = cm_unixsocket_client_recv(h, sdata, &lens);
			    if (ret < 0) {
				    if (errno == EAGAIN || errno == EWOULDBLOCK) {
				    	usleep(500000);
				    	continue;
				    }
			    	printf("recv error [%d][%s]\n", errno, strerror(errno));
			    	break;
			    } else {
			    	printf("recv data [%d][%s]\n", lens, sdata);
					break;
			    }
			}
			sleep(1);
		}
		cm_unixsocket_client_deinit(&h);
	}
	return 0;
}
