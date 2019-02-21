/*================================================================
*   Copyright (C) 2018 FREEDOM Ltd. All rights reserved.
*   
*   文件名称：busserver.c
*   创 建 者：chenjie.gu
*   创建日期：2018年05月23日
*   描    述：
*
================================================================*/


#include "busserver.h"
#include "mongoose.h"

#define BUSCLIENT_KEEPALIVE         1
#define BUSCLIENT_RPC_REQUEST       2
#define BUSCLIENT_RPC_RESPONSE      3
#define BUSCLIENT_SEND_TOPIC        4
#define BUSCLIENT_RECV_TOPIC        5

#define LOG(fmt, ...) \
    do {  \
        printf("======== %s():%d ===> ", __func__, __LINE__); \
        printf(fmt, ##__VA_ARGS__); \
    } while(0);

struct mg_connection *conn = NULL;
event_cb user_cb = NULL;
void *user = NULL;

enum {
    BUS_TYPE_SCTEK = 0, // 50001
    BUS_TYPE_DLNA = 1,  // 50002
    BUS_TYPE_BT = 2,    // 50003
    BUS_TYPE_BTAVK = 3,  // 50004
    BUS_TYPE_AISPEECH = 4,   // 50005
    BUS_TYPE_MAX = 5,
};

struct bus_clients {
    int                   type;   // type of which program
    int                   port;   // which port to listen
    struct mg_connection *conn;
};

static struct bus_clients g_bus_clients[BUS_TYPE_MAX];

struct message {
    struct multipart bm;
};

static int bus_client_init() {
    int i = 0;
    for (i = 0; i < BUS_TYPE_MAX; i++) {
        g_bus_clients[i].conn = NULL;
        g_bus_clients[i].port = 50001+i;
        g_bus_clients[i].type = i;
    }
    return 0;
}

static int bus_client_set_conn(int type, struct mg_connection *conn) {
    if (type < 0 || type >= BUS_TYPE_MAX) return -1;
    g_bus_clients[type].conn = conn;
    return 0;
}

static int find_in_str(const char *p)
{
    char *pos = strstr(p, "\r\n");
    if (pos == NULL) {
        return -1;
    }
    return pos - p;
}

static int busclient_parse_multipart_msg(struct mg_connection *nc, struct message *msg, int *type)
{
	const char *pp = nc->recv_mbuf.buf;
	const char *p = pp;
	int len = nc->recv_mbuf.len;
	int handle_len = 0, reserve_len = len, part_len = 0, _len = 0, n = 0;

	for (int i = 0; i < BUSCLIENT_MAX_MULTIPART_OPT; ++i) {
		_len = find_in_str(p);
		part_len = atoi(p);
		if( reserve_len >= _len+2+part_len+2 && strncmp(p+_len,"\r\n",2)==0 && strncmp(p+_len+2+part_len, "\r\n", 2) == 0) {
			msg->bm.part[n].len = part_len;
			msg->bm.part[n].data = p + _len + 2;
			n++;
			msg->bm.n = n;
		} else {
			return -1;
		}
		handle_len = handle_len + _len + 2 + part_len + 2;
		reserve_len = len - handle_len;
		p = pp + handle_len;

		if (reserve_len >=2 && strncmp(p, "\r\n", 2) == 0) {
			if (strncmp(msg->bm.part[0].data, "keepalive", strlen("keepalive")) == 0) {
				*type = BUSCLIENT_KEEPALIVE;
			}
			if (strncmp(msg->bm.part[0].data, "request", strlen("request")) == 0) {
				*type = BUSCLIENT_RPC_REQUEST;
			}
			if (strncmp(msg->bm.part[0].data, "response", strlen("response")) == 0) {
				*type = BUSCLIENT_RPC_RESPONSE;
			}
			if (strncmp(msg->bm.part[0].data, "publish", strlen("publish")) == 0) {
				*type = BUSCLIENT_RECV_TOPIC;
			}
			return handle_len + 2;
		}
	}

	return -1;
}

static void busclient_mg_send(struct mg_connection *nc, const char *data, int len)
{
    char tmp[10] = { 0 };
    int _len = 0;

    _len = sprintf(tmp, "%d", len);
    mg_send(nc, tmp, _len);
    mg_send(nc, "\r\n", 2);
    mg_send(nc, data, len);
    mg_send(nc, "\r\n", 2);
}

static void busclient_multipart_send(struct mg_connection *nc, struct multipart *bm) {
    for (int i = 0; i < bm->n; i++) {
        busclient_mg_send(nc, bm->part[i].data, bm->part[i].len);
    }
    mg_send(nc, "\r\n", 2);
}


static void handle_multipart_msg(struct mg_connection *nc, struct message *msg) {
    if (!strncmp(msg->bm.part[0].data, "request", msg->bm.part[0].len)) {
        if (!strncmp(msg->bm.part[1].data, "/bus/join", msg->bm.part[1].len)) {
            LOG("busserver recv message request [%s], from [%s], nc [%p]\n", msg->bm.part[1].data,
                msg->bm.part[2].data, nc);
                
            if (!strncmp(msg->bm.part[2].data, "sctek", msg->bm.part[2].len)) {
                bus_client_set_conn(BUS_TYPE_SCTEK, nc);
            } else if (!strncmp(msg->bm.part[2].data, "dlna", msg->bm.part[2].len)) {
                bus_client_set_conn(BUS_TYPE_DLNA, nc);
            } else if (!strncmp(msg->bm.part[2].data, "bt", msg->bm.part[2].len)) {
                bus_client_set_conn(BUS_TYPE_BT, nc);
            } else if (!strncmp(msg->bm.part[2].data, "btavk", msg->bm.part[2].len)) {
                bus_client_set_conn(BUS_TYPE_BTAVK, nc);
            } else if (!strncmp(msg->bm.part[2].data, "aispeech", msg->bm.part[2].len)) {
                bus_client_set_conn(BUS_TYPE_AISPEECH, nc);
            }
            struct multipart m = {1, {
                {strlen("response"), "response"}
            }};

            busclient_multipart_send(nc, &m);
        }
        else if (!strncmp(msg->bm.part[1].data, "/bus/subscribe", msg->bm.part[1].len)) {
            struct multipart m = {1, {
                {strlen("response"), "response"}
            }};

            busclient_multipart_send(nc, &m);
        }
    }

    else if (!strncmp(msg->bm.part[0].data, "publish", msg->bm.part[0].len)) {
        char tmptopic[1024] = {0};
        strncpy(tmptopic, msg->bm.part[1].data, msg->bm.part[1].len);
        if (user_cb) {
            user_cb(tmptopic, msg->bm.part[2].data, msg->bm.part[2].len, user);
        }
    }
}

static void ev_handler(struct mg_connection *nc, int ev, void *p) {
  struct mbuf *io = &nc->recv_mbuf;
  (void) p;

  if (ev != 0) LOG("busserver event back ev [%d]\n", ev);
  switch (ev) {
    case MG_EV_ACCEPT:
      break;
    case MG_EV_RECV:
        conn = nc;
      // receive multipart
      int len = 0, type = 0;
      struct message msg;
      while (1) {
          len = busclient_parse_multipart_msg(nc, &msg, &type);
          if (len == -1 || type == 0) break;
          handle_multipart_msg(nc, &msg);
          mbuf_remove(&nc->recv_mbuf, len);
      }
      break;
      
    case MG_EV_TIMER:
      {
          struct multipart *m = (struct multipart *)p;
          busclient_multipart_send(nc, m);
          break;
      }
      
    default:
      break;
  }
}

void busserver_run_ex(event_cb cb, void *u) {
    bus_client_init();
    //
    struct mg_mgr mgr;
    mg_mgr_init(&mgr, NULL);

    // Acturely , we only need listen one port, then let client register with name 
    // to differentiate them.
    int i = 0;
    for (i = 0; i < 1; i++) {
        int port = g_bus_clients[i].port;
        char listen[32] = {0};
        sprintf(listen, "%s:%d", "0.0.0.0", port);
        struct mg_connection *tmpconn = mg_bind(&mgr, listen, ev_handler);
        if (!tmpconn) {
            printf("busserver listen failed of index [%d], port [%d]\n", i, port);
        } else {
            LOG("busserver listen on [%s]\n", listen);
        }
    }

    user_cb = cb;
    user = u;

    for (;;) {
        mg_mgr_poll(&mgr, 100);
    }

    mg_mgr_free(&mgr);
}

void busserver_run(const char *addr, event_cb cb, void *u) {
    //
    struct mg_mgr mgr;
    mg_mgr_init(&mgr, NULL);
    mg_bind(&mgr, addr, ev_handler);

    user_cb = cb;
    user = u;

    for (;;) {
        mg_mgr_poll(&mgr, 100);
    }

    mg_mgr_free(&mgr);
}

int busserver_broadcast(const char *topic, const char *data) {
    if (!topic) return -1;

    struct multipart m = {3, {
        {strlen("publish"), "publish"},
            {strlen(topic), topic},
            {strlen(data), data}
    }};

    int i = 0;
    for (i = 0; i < BUS_TYPE_MAX; i++) {
        if (g_bus_clients[i].conn) {
            ev_handler(g_bus_clients[i].conn, MG_EV_TIMER, &m);
        } else {
            printf("busserver broadcast with no conn type [%d]\n", i);
        }
    }

    return 0;
}

int busserver_send_msg(const char *topic, const char *data) {

    if (!topic) return -1;

    struct multipart m = {3, {
        {strlen("publish"), "publish"},
            {strlen(topic), topic},
            {strlen(data), data}
    }};

    if (conn)
        ev_handler(conn, MG_EV_TIMER, &m);

    return 0;
}

int busserver_send_msg_to(const char *to, const char *topic, const char *data) {
    if (!topic) return -1;

    struct multipart m = {3, {
        {strlen("publish"), "publish"},
            {strlen(topic), topic},
            {strlen(data), data}
    }};

    struct mg_connection *c = NULL;
    if (!strcmp(to, "aispeech")) {
        c = g_bus_clients[BUS_TYPE_AISPEECH].conn;
    } else if (!strcmp(to, "sctek")) {
        c = g_bus_clients[BUS_TYPE_SCTEK].conn;
    } else if (!strcmp(to, "dlna")) {
        c = g_bus_clients[BUS_TYPE_DLNA].conn;
    } else if (!strncmp(to, "btavk", 5)) {
        c = g_bus_clients[BUS_TYPE_BTAVK].conn;
    } else if (!strncmp(to, "bt", 2)) {
        c = g_bus_clients[BUS_TYPE_BT].conn;
    }
    printf("busserver send message to [%s] of conn [%p]\n", to, c);
    if (c)
        ev_handler(c, MG_EV_TIMER, &m);

    return 0;
}

struct mg_connection *conn_client = NULL;
struct mg_mgr mgr_client;
int iClientExit = 0;
event_cb  client_cb = NULL;
static char bus_client_id[64] = "busdefault";

static void handle_multipart_msg_client(struct mg_connection *nc, struct message *msg) {
    if (!strncmp(msg->bm.part[0].data, "request", msg->bm.part[0].len)) {
    } else if (!strncmp(msg->bm.part[0].data, "publish", msg->bm.part[0].len)) {
        if (client_cb) {
            char topic_data[1024] = {0};
            strncpy(topic_data, msg->bm.part[1].data, msg->bm.part[1].len);
            client_cb(topic_data, msg->bm.part[2].data, msg->bm.part[2].len, user);
        } else {
            printf("client_cb is not set.....\n");
        }
    }
}

static void ev_handler_client(struct mg_connection *nc, int ev, void *p) {
  struct mbuf *io = &nc->recv_mbuf;
  (void) p;

  if (ev != 0) LOG("busclient [%p] event back ev [%d]\n", conn_client, ev);
  switch(ev) {
      case MG_EV_CONNECT:
         conn_client = nc;
         busclient_send_request("/bus/join", bus_client_id);
        //   mg_printf(nc, "%s", "hi friend");
      break;
      case MG_EV_RECV:
      {
            // receive multipart
        int len = 0, type = 0;
        struct message msg;
        while (1) {
            len = busclient_parse_multipart_msg(nc, &msg, &type);
            if (len == -1 || type == 0) break;
            handle_multipart_msg_client(nc, &msg);
            mbuf_remove(&nc->recv_mbuf, len);
        }
      }
      break;
      case MG_EV_CLOSE:
          iClientExit = 2;
      break;
      default:
      break;
  }
}

// client
int busclient_connect(const char *id, const char *addr, event_cb cb, void *u) {
    strncpy(bus_client_id, id, sizeof(bus_client_id));
    mg_mgr_init(&mgr_client, NULL);
    conn_client = mg_connect(&mgr_client, addr, ev_handler_client);
    if (conn_client == NULL) {
        return -1;
    }
    client_cb = cb;
    iClientExit = 0;
    while (!iClientExit) {
        mg_mgr_poll(&mgr_client, 10);
    }
    mg_mgr_free(&mgr_client);
    return iClientExit == 2 ? 1 : -1;
}

int busclient_send_request(const char *topic, const char *data) {
    struct multipart m = {3, {
        {strlen("request"), "request"},
            {strlen(topic), topic},
            {strlen(data), data}
    }};
    if (conn_client) {
        ev_handler(conn_client, MG_EV_TIMER, &m);
    } else {
        printf("error busclient have not connected.\n");
    }
    return 0;
}

int busclient_send_msg(const char *topic, const char *data) {
    struct multipart m = {3, {
        {strlen("publish"), "publish"},
            {strlen(topic), topic},
            {strlen(data), data}
    }};
    if (conn_client) {
        ev_handler(conn_client, MG_EV_TIMER, &m);
    } else {
        printf("error busclient have not connected.\n");
    }
    return 0;
}
