//
// Created by v on 16-3-2.
// Author: jmdvirus@roamter.com
//

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "rt_unixsocket.h"
#include "rt_print.h"

////
// Demo of server

int rt_demo_unixsocket_server() {
    RTUnixSocketServerHandle h;
    int ret = rt_unixsocket_server_init(&h, "/tmp/kfupload.sock");
    if (ret < 0) {
        rterror("init failed: %d", ret);
        return -1;
    }
    rtdebug("start to accept client.")

    int bexit = 0;
    while (!bexit) {
        int client = rt_unixsocket_server_accept(h);
        int bread = 1;
        rtdebug("start to read data from client.")
        while (bread) {
            char data[2048] = {0};
            int len = 2048;
            int n = rt_unixsocket_server_recv(h, client, data, &len);
            rtinfo("recv data: %d, [%s]", n, data);
            if (n <= 0) {
                rt_unixsocket_server_client_close(client);
                bread = 0;
            }
            usleep(500000);
        }
        sleep(1);
    }

    rtinfo("exit program");
    return 0;
}

int rt_demo_unixsocket_client() {
    RTUnixSocketClientHandle  h;
    int ret = rt_unixsocket_client_init(&h, "/tmp/kfupload.sock");
    if (ret < 0) {
        rterror("init client socket failed: %d", ret);
        return -1;
    }

    int maxcnts = 0;
    int curcnts = 0;
    int bexit = 0;
    while (!bexit) {
        if (curcnts++ > maxcnts) break;
        char data[1024] = "{\"type\":\"http\",\"device_id\":\"20f41bf69886\",\"flow_id\":\"20f41bf6988    6_20160303173708_192.168.1.9_54968_199.59.150.44_80_990364\",\"timestamp\":\"1456997828\",\"src_ip\":\"192.168.1.9\",\"src_port\":\"54968\",\"dst_ip\":\"199.59.150.44\",\"dst_port\":\"80\",\"hostname\":\"\",\"url\":\"/\",\"status\":\"\",\"method\":\"\",\"content_t    ype\":\"\",\"content_length\":\" 3154^M\",\"agent\":\"\",\"referer\":\"\",\"title\":\"\",\"keywords\":\"\"}\n{\"type\":\"http\",\"device_id\":\"20f41bf69886\",\"flow_id\":\"20f41bf69886_20160303173708_192.168.1.9_54968_199.59.150.44_80_990364\",\"timestamp\":\"1456997828\",\"src_ip\":\"192.168.1.9\",\"src_port\":\"54968\",\"dst_ip\":\"199.59.150.44\",\"dst_port\"    :\"80\",\"hostname\":\"\",\"url\":\"/\",\"status\":\"\",\"method\":\"\",\"content_type\":\"\",\"content_length\":\"\",\"agent\":\"\",\"referer\":\"\",\"title\":\"\",\"keywords\":\"\"}";
        int len = strlen(data);
        int n = rt_unixsocket_client_send(h, data, len);
        if (n < 0) {
            rterror("send failed: %d", n);
            bexit = 1;
        }
        rtinfo("send data out: %d", n);
        usleep(2000000);
    }

    return 0;
}

int main(int argc, char **args) {
    char *type = "client";
    if (argc >= 2) {
        type = args[1];
    }
    if (strcmp(type, "client") == 0) {
        rt_demo_unixsocket_client();
    } else if (strcmp(type, "server") == 0) {
        rt_demo_unixsocket_server();
    }

    return 0;
}