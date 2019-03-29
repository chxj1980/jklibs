/*********************************************************
 *
 * Filename: cmbusclient.c
 *   Author: jmdvirus
 *   Create: 2019年01月07日 星期一 16时13分30秒
 *
 *********************************************************/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include "busserver_ex.h"

static CMBusclientP p1;
static CMBusclientP p2;

static pthread_t  pp1;
static pthread_t  pp2;

void c_cb(const char *topic, const char *topic_data, int data_len, void *user)
{
	printf("c_cb [%p] data [%s]\n", user, topic_data);
}

void *p_thread_1(void *args)
{
	int x = 1;
	int ret = cm_busclient_init(&p1, "demo1", "0.0.0.0", 50001, &c_cb, &x);
	if (ret < 0) {
		printf("error init demo1\n");
		return NULL;
	}
	printf("go to connect demo1\n");
again:
	ret = cm_busclient_connect(p1);
	if (ret < 0) {
		sleep(2);
		printf("Error, again connect\n");
		goto again;
	}
	printf("demo 1 exit\n");
	cm_busclient_deinit(&p1);
	return NULL;
}

void *p_thread_2(void *args)
{
	int x = 2;
	int ret = 0;
	again:
	ret = cm_busclient_init(&p2, "demo2", "0.0.0.0", 50001, &c_cb, &x);
	if (ret < 0) {
		printf("error init demo2\n");
		return NULL;
	}
	printf("go to connect demo\n");
	ret = cm_busclient_connect(p2);
	if (ret < 0) {
		sleep(2);
		printf("Error, again connect\n");
		cm_busclient_deinit(&p2);
		goto again;
	}
	printf("demo 2 exit\n");
	cm_busclient_deinit(&p2);
	return NULL;
}

int main(int argc, char **args) {
	int ret = pthread_create(&pp1, NULL, p_thread_1, NULL);
	ret = pthread_create(&pp2, NULL, p_thread_2, NULL);

	sleep(5);
	cm_busclient_send_msg(p1, "/move/left", "speedfast");
	sleep(1);
	cm_busclient_send_msg(p2, "/move/right", "speedslow");
	sleep(4);
	cm_busclient_send_msg(p2, "/hello/world", "nihao");
	
	pthread_join(pp1, NULL);
	pthread_join(pp2, NULL);
	printf("main exit\n");

	return 0;
}

