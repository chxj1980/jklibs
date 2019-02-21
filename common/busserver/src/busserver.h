#ifndef BUSSERVER_H_
#define BUSSERVER_H_

#ifdef __cplusplus
extern "C" {
#endif

struct bc_str {
	int len;
	const char *data;
};

#define BUSCLIENT_MAX_MULTIPART_OPT 20

struct multipart {
	int n;
    // int capacity;
	struct bc_str part[BUSCLIENT_MAX_MULTIPART_OPT];
};

typedef void (*event_cb)(const char *topic, const char *topic_data, int data_len, void *user);

void busserver_run(const char *addr, event_cb, void *user);

// custom listen, listen more client.
void busserver_run_ex(event_cb cb, void *u);

int busserver_send_msg(const char *topic, const char *data);
// @to : aispeech, sctek, btavk, bt, dlna
int busserver_send_msg_to(const char *to, const char *topic, const char *data);

// broadcast message to every connected clients.
int busserver_broadcast(const char *topic, const char *data);

// client operation
// only support in one progress

int busclient_send_request(const char *topic, const char *data);
int busclient_connect(const char *id, const char *addr, event_cb cb, void *u);
int busclient_send_msg(const char *topic, const char *data);

#ifdef __cplusplus
}
#endif
#endif

