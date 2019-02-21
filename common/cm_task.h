/*********************************************************
 *
 * Filename: cm/cm_task.h
 *   Author: jmdvirus
 *   Create: 2018年11月29日 星期四 14时27分08秒
 *
 *********************************************************/

#ifndef __COMMON_TASK__
#define __COMMON_TASK__

#include <stdio.h>
#include "cm_list.h"

typedef int (*cm_task_run)(void *args);

struct cm_task {
    struct list_head head;
    cm_task_run      run;
	void             *args;
	unsigned long long    inter;
};

int cm_task_init();

int cm_task_deinit();

int cm_task_start(struct cm_task *task, unsigned long long inter, cm_task_run run, void *args);

int cm_task_cancel(struct cm_task *task);

int cm_task_poll();

#endif

