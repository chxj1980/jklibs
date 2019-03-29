/*********************************************************
 *
 * Filename: base/cm_local_debug.h
 *   Author: jmdvirus
 *   Create: 2018年12月03日 星期一 14时36分40秒
 *
 *********************************************************/

#include <pthread.h>
#include <semaphore.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "cm_conn_udp.h"

typedef int (*on_recvdata)(const char *data, int len, void *userdata);

typedef struct _CMLocalDebug {
	CMConnUDP         pHD;
	int               iPort;
	on_recvdata       cbRecvData;
	void              *pUserdata;

	pthread_t         iThread;
	int               iExit;
	sem_t             iSem;
} CMLocalDebug;

int cm_local_debug_init(CMLocalDebug *ld, int port, on_recvdata recvdata, void *userdata);

int cm_local_debug_send(CMLocalDebug *ld, const char *data, int len);

int cm_local_debug_deinit(CMLocalDebug *ld);

#ifdef __cplusplus
}
#endif

