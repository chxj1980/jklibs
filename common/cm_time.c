/*********************************************************
 *
 * Filename: cm/cm_time.c
 *   Author: jmdvirus
 *   Create: 2019年01月02日 星期三 20时45分04秒
 *
 *********************************************************/

#include <stdio.h>

#include "cm_time.h"

int cm_timewait_start(CM_TimeWait *tw, int duration)
{
	if (!tw) return -1;
	if (duration < 0) return -2;

	tw->iDuration = duration;
	tw->ullLastTime = cm_gettime_milli();
	tw->iStart = 1;
	return 0;
}

int cm_timewait_stop(CM_TimeWait *tw)
{
	if (!tw) return -1;
    tw->iStart = 0;
	tw->ullLastTime = 0;
	return 0;
}

int cm_timewait_check(CM_TimeWait *tw)
{
	if (!tw || !tw->iStart) return -1;
	unsigned long long now = cm_gettime_milli();
    if (now - tw->ullLastTime > (unsigned long long) tw->iDuration) {
		tw->ullLastTime = now;
		return 1;
	}
	return 0;
}


