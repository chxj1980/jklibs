//
// Created by v on 18-7-2.
//

#ifndef PROJECT_CM_HTTP_UPLOAD_H
#define PROJECT_CM_HTTP_UPLOAD_H

#include "cm_http.h"
#include "cm_conn_tcp.h"

#ifdef __cplusplus
extern "C" {
#endif

int cm_http_send_file(char *filename, char *url, int subid);
int cm_http_send_file_two(char *filename, char *url, const char* imei);

typedef struct {
    CMHttpMessage  szMsg;
	char           szUploadFileKeyName[64];
    CMConnTCP      szConn;
    CMHttpExt      szExt[16];
    char           szResponse[10240];
	char           szResContent[4096];
} CMHttpUpload;

int cm_http_send_base(CMHttpUpload *up, const char *url, const char *path, const char *filename);

#ifdef __cplusplus
}
#endif
#endif //PROJECT_CM_HTTP_UPLOAD_H
