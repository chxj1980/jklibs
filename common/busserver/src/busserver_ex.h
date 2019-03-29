#ifndef BUSSERVER_H_
#define BUSSERVER_H_

#ifdef __cplusplus
extern "C" {
#endif

struct bc_str {
	int len;
	const char *data;
};

typedef struct _CMBusserver *CMBusserverP;

#define BUSCLIENT_MAX_MULTIPART_OPT 20

struct multipart {
	int n;
    // int capacity;
	struct bc_str part[BUSCLIENT_MAX_MULTIPART_OPT];
};

typedef void (*event_cb)(const char *topic, const char *topic_data, int data_len, void *user);

int cm_busserver_init(CMBusserverP *ss, void *priv, int port, event_cb cb);
int cm_busserver_deinit(CMBusserverP *ss);

int cm_busserver_run(CMBusserverP ss);

int cm_busserver_send_msg(CMBusserverP ss, const char *to, const char *topic, const char *data);
int cm_busserver_broadcast(CMBusserverP ss, const char *topic, const char *data);

//
// client
// 
typedef struct _CMBusclient *CMBusclientP;

int cm_busclient_init(CMBusclientP *cc, const char *from, const char *addr, int port, event_cb cb, void *priv);
int cm_busclient_deinit(CMBusclientP *cc);

// return < 0 error
//        = 0 normal exit
int cm_busclient_connect(CMBusclientP cc);

int cm_busclient_send_msg(CMBusclientP cc, const char *topic, const char *data);
int cm_busclient_send_request(CMBusclientP cc, const char *topic, const char *data);

#ifdef __cplusplus
}
#endif
#endif

