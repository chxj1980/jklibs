/*================================================================
*   Copyright (C) 2018 FREEDOM Ltd. All rights reserved.
*   
*   文件名称：busserver.c
*   创 建 者：chenjie.gu
*   创建日期：2018年05月23日
*   描    述：
*
================================================================*/


#include "busserver_ex.h"
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

enum {
    BUS_TYPE_SCTEK = 0, // 50001
    BUS_TYPE_DLNA = 1,  // 50002
    BUS_TYPE_BT = 2,    // 50003
    BUS_TYPE_BTAVK = 3,  // 50004
    BUS_TYPE_AISPEECH = 4,   // 50005
    BUS_TYPE_MAX = 5,
};

struct bus_clients {
	char                  name[32];
    struct mg_connection *conn;
};

struct _CMBusserver {
	int                      port; // listen port
	struct bus_clients       clients[BUS_TYPE_MAX];
    struct mg_mgr            mgr;
	event_cb                 cb;
	void                     *priv;
};

struct message {
    struct multipart bm;
};

static int bus_client_set_conn(CMBusserverP ss, const char *name, struct mg_connection *conn) {
	if (!ss) return -1;
	int i = 0;
	for (i = 0; i < BUS_TYPE_MAX; i++) {
		if (strncmp(ss->clients[i].name, name, strlen(name)) == 0) {
			LOG("Find client exist of index [%d], conn [%p] of name [%s]\n", i, conn, name);
			strncpy(ss->clients[i].name, name, sizeof(ss->clients[i].name) -1);
			ss->clients[i].conn = conn;
			return 0;
		}
	}
	for (i = 0; i < BUS_TYPE_MAX; i++) {
		if (ss->clients[i].name[0] == '\0') {
            strncpy(ss->clients[i].name, name, sizeof(ss->clients[i].name) -1);
            ss->clients[i].conn = conn;
			LOG("Find client of index [%d], conn [%p] of name [%s]\n", i, conn, name);
			break;
		}
	}
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


static void handle_multipart_msg(CMBusserverP ss, struct mg_connection *nc, struct message *msg) {
	if (!ss) return;
    if (!strncmp(msg->bm.part[0].data, "request", msg->bm.part[0].len)) {
        if (!strncmp(msg->bm.part[1].data, "/bus/join", msg->bm.part[1].len)) {
            LOG("busserver recv message request [%s], from [%s], nc [%p]\n", msg->bm.part[1].data,
                msg->bm.part[2].data, nc);
                
			char name[32] = {0};
			strncpy(name, msg->bm.part[2].data, 
					msg->bm.part[2].len > sizeof(name) ? sizeof(name) -1 : msg->bm.part[2].len);
			bus_client_set_conn(ss, name, nc);
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
        if (ss->cb) {
            ss->cb(tmptopic, msg->bm.part[2].data, msg->bm.part[2].len, ss->priv);
        }
    }
}

static void ev_handler(struct mg_connection *nc, int ev, void *p) {
  struct mbuf *io = &nc->recv_mbuf;

  CMBusserverP ss = (CMBusserverP)nc->user_data;
  int i = 0;

  if (ev != 0) LOG("busserver event back ev [%d] of [%p] conn [%p]\n", ev, nc->user_data, nc);
  switch (ev) {
    case MG_EV_ACCEPT:
      break;
    case MG_EV_RECV:
	  {
      // receive multipart
      int len = 0, type = 0;
      struct message msg;
      while (1) {
          len = busclient_parse_multipart_msg(nc, &msg, &type);
          if (len == -1 || type == 0) break;
          handle_multipart_msg(ss, nc, &msg);
          mbuf_remove(&nc->recv_mbuf, len);
      }
	  }
      break;

	case MG_EV_CLOSE:
	  for (i = 0; i < BUS_TYPE_MAX; i++) {
		  if (ss->clients[i].conn == nc) {
			  LOG("Find exist client close index [%d] conn [%p]\n", i, nc);
			  ss->clients[i].name[0] = '\0';
			  ss->clients[i].conn = NULL;
			  break;
		  }
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

int cm_busserver_init(CMBusserverP *ss, void *priv, int port, event_cb cb)
{
	CMBusserverP in = (CMBusserverP)malloc(sizeof(struct _CMBusserver));
	if (!in) return -1;

    mg_mgr_init(&in->mgr, priv);

    char listen[32] = {0};
    sprintf(listen, "%s:%d", "0.0.0.0", port);

    struct mg_connection *tmpconn = mg_bind(&in->mgr, listen, ev_handler);
    if (!tmpconn) {
        LOG("busserver listen failed of index port [%d]\n", port);
		free(in);
		return -1;
    } else {
        LOG("busserver listen on [%s] with ss [%p]\n", listen, in);
    }
    tmpconn->user_data = in;
	in->priv = priv;
	in->cb = cb;

	*ss = in;
	return 0;
}

int cm_busserver_deinit(CMBusserverP *ss)
{
	if (!ss) return -1;

    mg_mgr_free(&(*ss)->mgr);
	free(*ss);
	*ss = NULL;
	return 0;
}

int cm_busserver_run(CMBusserverP ss)
{
    for (;;) {
        mg_mgr_poll(&ss->mgr, 100);
    }
	return 0;
}

int cm_busserver_broadcast(CMBusserverP ss, const char *topic, const char *data) {
    if (!ss || !topic) return -1;

    struct multipart m = {3, {
        {strlen("publish"), "publish"},
            {strlen(topic), topic},
            {strlen(data), data}
    }};

    int i = 0;
    for (i = 0; i < BUS_TYPE_MAX; i++) {
        if (ss->clients[i].conn) {
            LOG("busserver broadcast send data to [%s] with conn [%p]\n", ss->clients[i].name, ss->clients[i].conn);
            ev_handler(ss->clients[i].conn, MG_EV_TIMER, &m);
        } else {
            LOG("busserver broadcast with no conn type [%d]\n", i);
        }
    }

    return 0;
}

int cm_busserver_send_msg(CMBusserverP ss, const char *to, const char *topic, const char *data) {

    if (!ss || !topic) return -1;

    struct multipart m = {3, {
        {strlen("publish"), "publish"},
            {strlen(topic), topic},
            {strlen(data), data}
    }};

	struct mg_connection *c = NULL;
	int i = 0;
    for (i = 0; i < BUS_TYPE_MAX; i++) {
		if (strncmp(to, ss->clients[i].name, strlen(to)) == 0) {
			c = ss->clients[i].conn;
			break;
		}
	}
    if (c) {
        LOG("busserver send data to [%s] with conn [%p]\n", to, c);
        ev_handler(c, MG_EV_TIMER, &m);
    } else {
        LOG("busserver not find conn of name [%s]\n", to);
    }

    return 0;
}

struct _CMBusclient {
	char                 name[32];
	char                 addr[64];
	int                  iPort;
    struct mg_connection *conn;
	struct mg_mgr        mgr_client;
	int                  iExit;
	event_cb             cb;
	void                 *priv;
};

int cm_busclient_init(CMBusclientP *cc, const char *from, const char *addr, int port, event_cb cb, void *priv)
{
	CMBusclientP in = (CMBusclientP)malloc(sizeof(struct _CMBusclient));
	if (!in) return -1;

    mg_mgr_init(&in->mgr_client, in);
	strncpy(in->name, from, sizeof(in->name) - 1);
	strncpy(in->addr, addr, sizeof(in->addr) - 1);
	in->iPort = port;
	in->cb = cb;
	in->priv = priv;

	in->mgr_client.user_data = in;

	LOG("busclient init done [%p]\n", in);

	*cc = in;

	return 0;
}

int cm_busclient_deinit(CMBusclientP *cc)
{
	if (!cc) return -1;
	CMBusclientP icc = *cc;
    mg_mgr_free(&icc->mgr_client);

	free(*cc);
	*cc = NULL;
	return 0;
}

static void handle_multipart_msg_client(CMBusclientP cc, struct mg_connection *nc, struct message *msg) {
    if (!strncmp(msg->bm.part[0].data, "request", msg->bm.part[0].len)) {
    } else if (!strncmp(msg->bm.part[0].data, "publish", msg->bm.part[0].len)) {
        if (cc->cb) {
            char topic_data[1024] = {0};
            strncpy(topic_data, msg->bm.part[1].data, msg->bm.part[1].len);
            cc->cb(topic_data, msg->bm.part[2].data, msg->bm.part[2].len, cc->priv);
        } else {
            printf("client_cb is not set.....\n");
        }
    }
}

static void ev_handler_client(struct mg_connection *nc, int ev, void *p) {
  struct mbuf *io = &nc->recv_mbuf;
  CMBusclientP cc = (CMBusclientP)nc->user_data;

  if (ev != 0) LOG("busclient [%p] event back ev [%d]\n", nc->user_data, ev);
  switch(ev) {
      case MG_EV_CONNECT:
         cc->conn = nc;
         cm_busclient_send_request(cc, "/bus/join", cc->name);
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
            handle_multipart_msg_client(cc, nc, &msg);
            mbuf_remove(&nc->recv_mbuf, len);
        }
      }
      break;
      case MG_EV_CLOSE:
          cc->iExit = 2;
      break;
      default:
      break;
  }
}

// client
int cm_busclient_connect(CMBusclientP cc) {
	char addr[128] = {0};
	sprintf(addr, "%s:%d", cc->addr, cc->iPort);
    cc->conn = mg_connect(&cc->mgr_client, addr, ev_handler_client);
    if (cc->conn == NULL) {
        return -1;
    }
    cc->conn->user_data = cc;
    while (!cc->iExit) {
        mg_mgr_poll(&cc->mgr_client, 10);
    }
    int ret = cc->iExit == 2 ? -2 : 0;
    cc->iExit = 0;
	return ret;
}

int cm_busclient_send_request(CMBusclientP cc, const char *topic, const char *data) {
	if (!cc) return -1;
    struct multipart m = {3, {
        {strlen("request"), "request"},
            {strlen(topic), topic},
            {strlen(data), data}
    }};
    if (cc->conn) {
        ev_handler(cc->conn, MG_EV_TIMER, &m);
    } else {
        printf("error busclient have not connected.\n");
    }
    return 0;
}

int cm_busclient_send_msg(CMBusclientP cc, const char *topic, const char *data) {
	if (!cc) return -1;
    struct multipart m = {3, {
        {strlen("publish"), "publish"},
            {strlen(topic), topic},
            {strlen(data), data}
    }};
    if (cc->conn) {
        ev_handler(cc->conn, MG_EV_TIMER, &m);
    } else {
        printf("error busclient have not connected.\n");
    }
    return 0;
}
