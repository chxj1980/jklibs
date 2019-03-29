/*********************************************************
 *
 * Filename: cm/cm_task.c
 *   Author: jmdvirus
 *   Create: 2018年11月29日 星期四 14时31分42秒
 *
 *********************************************************/

#include <stdio.h>
#include <pthread.h>

#include "cm_task.h"
#include "cm_utils.h"

static struct cm_task g_task_list;

static pthread_mutex_t  list_mutex = PTHREAD_MUTEX_INITIALIZER;

int cm_task_init()
{
    cm_init_list_head(&g_task_list.head);

	return 0;
}

int cm_task_deinit()
{
	struct list_head *tmp, *p;
	pthread_mutex_lock(&list_mutex);
	cm_list_each_safe(tmp, p, &g_task_list.head) {
		cm_list_del_item(tmp);
	}
	pthread_mutex_unlock(&list_mutex);

	return 0;
}

int cm_task_start(struct cm_task *task, unsigned long long inter, cm_task_run run, void *args)
{
	if (!task) return -1;
	if (!run) return -2;
	struct list_head *l = &task->head;
	cm_init_list_head(l);

	task->inter = inter + cm_gettime_milli();
	task->run = run;
	task->args = args;

	pthread_mutex_lock(&list_mutex);
	cm_list_add_front(l, &g_task_list.head);
	pthread_mutex_unlock(&list_mutex);

	return 0;
}

int cm_task_cancel(struct cm_task *task)
{
	if (!task) return -1;
	pthread_mutex_lock(&list_mutex);
	struct list_head *l = &task->head;
	cm_list_del_item(l);
	pthread_mutex_unlock(&list_mutex);

	return 0;
}

int cm_task_poll()
{
	struct list_head *tmp, *p;
	pthread_mutex_lock(&list_mutex);
	cm_list_each_safe(tmp, p, &g_task_list.head) {
		cm_task_run trun = NULL;
		void *args = NULL;
		struct cm_task *task = cm_list_item(tmp, struct cm_task, head);
		if (task) {
            unsigned long long now = cm_gettime_milli();
			if (now >= task->inter && task->run) {
				trun = task->run;
				args = task->args;
				cm_list_del_item(tmp);
			}
		}
		if (trun) {
			trun(args);
		}
	}
	pthread_mutex_unlock(&list_mutex);

	return 0;
}

#ifdef TASK_MAIN_TEST
#include <unistd.h>

struct cm_task t1;
struct cm_task t2;
int iRun = 1;

struct display1 {
    int          value;
};

int display_result1(void *args) {
	struct display1 *d1 = (struct display1*)args;
	printf("display 1 out [%d]\n", d1->value);
	if (d1->value == 2) {
		iRun = 0;
		//cm_task_cancel(&t2);
		return 0;
	}

	d1->value += 1;
	cm_task_start(&t1, 5000, &display_result1, d1);
	return 0;
}

int display_result2(void *args) {
	struct display1 *d2 = (struct display1*)args;
	printf("display 2 out [%d]\n", d2->value);
	if (d2->value == 105) {
		return 0;
	}
	d2->value += 1;
	cm_task_start(&t2, 3000, &display_result2, d2);
	return 0;
}

int main(int argc, char *args) {

	cm_task_init();

	printf("go to start one task\n");
	struct display1 d1;
	d1.value = 0;
	cm_task_start(&t1, 2000, &display_result1, &d1);

	struct display1 d2;
    d2.value = 100;
	cm_task_start(&t2, 2000, &display_result2, &d2);

	while(iRun) {
		cm_task_poll();
		usleep(50000);
	}
	cm_task_deinit();
	sleep(10);
	printf("Program exit...\n");

	return 0;
}
#endif


