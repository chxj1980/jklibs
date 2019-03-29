
#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include "cm_conn_tcp.h"
#include "cm_print.h"

CMLogPrint logPrint;

CMConnTCP tcph;

int main(int argc, char **args) {
    char *addr = "192.168.5.180";
    int port = 22224;
    int ret = cm_conn_tcp_create_new(&tcph, addr, port);
    if (ret < 0) {
        printf("Error timeout [%d]\n", ret);
        return 1;
    }

	char data[1024] = {0};
	long datalen = 1024;
	while (1) {
		int ret = cm_conn_tcp_recv_simple(tcph, data, &datalen);
		printf("recv out [%d]\n", ret);
		usleep(50000);
	}

    return 0;
}
