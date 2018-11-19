//
// Created by v on 18-9-12.
//

#ifndef SRC_CM_BROADCAST_H
#define SRC_CM_BROADCAST_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CMBroadcast *CMBroadcastHandle;

int cm_broadcast_set_info(CMBroadcastHandle h, const char *mac, const char *ip, const char *version);

int cm_broadcast_init(CMBroadcastHandle *h, int port);

int cm_broadcast_start(CMBroadcastHandle h);

int cm_broadcast_close(CMBroadcastHandle *h);

#ifdef __cplusplus
};
#endif

#endif //SRC_CM_BROADCAST_H
