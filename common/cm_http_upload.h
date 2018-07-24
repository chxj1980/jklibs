//
// Created by v on 18-7-2.
//

#ifndef PROJECT_CM_HTTP_UPLOAD_H
#define PROJECT_CM_HTTP_UPLOAD_H

#ifdef __cplusplus
extern "C" {
#endif

int cm_http_send_file(char *filename, char *url, int subid);

#ifdef __cplusplus
}
#endif
#endif //PROJECT_CM_HTTP_UPLOAD_H
