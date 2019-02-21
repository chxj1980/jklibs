cmake_minimum_required(VERSION 2.80)

set(BASE_FILE
        cm_print.c
        cm_conf.c
        cm_base64.c
        cm_list.c
        cm_flash_cfg.c
        cm_sys.c
        cm_task.c
        cm_time.c
        cm_uart.c
        cm_utils.c
        jkbytes.c
        ngx_md5.c)

set(NET_FILE
        cm_conn_tcp.c
        cm_conn_udp.c
        cm_conn_ws.c
        cm_http.c
        cm_http_download.c
        cm_http_upload.c
        cm_unixsocket.c
        http_parser.c)