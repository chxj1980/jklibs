/*********************************************************
 *
 * Filename: demo/demo_thread.c
 *   Author: jmdvirus
 *   Create: 2018年08月04日 星期六 09时31分01秒
 *
 *********************************************************/

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "cm_logprint.h"

CMLogPrint logPrint;

int thread_exit = 0;

void *test_result(void *args) 
{
	int indx = 0;

	while (!thread_exit) {
		time_t t = time(NULL);
		printf("[%ld]do something index [%d]\n", t, indx);
		indx++;
		sleep(2);
	}
	printf("Thread exit\n");
	return NULL;
}

int main() {
	pthread_t p;
	pthread_create(&p, NULL, test_result, NULL);

	int count = 0;
	while(1) {
		if (count++ > 0) {
			time_t t = time(NULL);
			printf("[%ld]Start to kill thread\n", t);
			//thread_exit = 1;
			//pthread_join(p, NULL);
			pthread_cancel(p);
			break;
		}
		sleep(2);
	}

	printf("program exit\n");
	sleep(1);

	return 0;
}

