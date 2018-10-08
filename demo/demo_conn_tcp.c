
#include <stdio.h>
#include <netdb.h>
#include "cm_conn_tcp.h"

CMConnTCP tcph;

int main(int argc, char **args) {
    char *addr = "d3-edu.com";
    int port = 8081;
    int ret = cm_conn_tcp_create_new_timeout(&tcph, addr, port, 5);
    if (ret < 0) {
        printf("Error timeout [%d]\n", ret);
        return 1;
    }

    return 0;
}