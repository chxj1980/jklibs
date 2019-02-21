/*********************************************************
 *
 * Filename: cm/misc/cm_suspend.c
 *   Author: jmdvirus
 *   Create: 2019年01月06日 星期日 11时01分46秒
 *
 *********************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cm_suspend.h"
#include "cm_logprint.h"
#include "cm_utils.h"

int cm_suspend_start(CMSuspend *sus, int duration)
{
	if (!sus) return -1;
	memset(sus, 0, sizeof(CMSuspend));
	sus->iDuration = duration > 0 ? duration : 5000;
	sus->ullLastTime = cm_gettime_milli();
	sus->iStart = 1;
	return 0;
}

int cm_suspend_stop(CMSuspend *sus)
{
	if (!sus) return -1;
    sus->iStart = 0;
	sus->iWakeUp = 0;
	return 0;
}

int cm_suspend_wake(CMSuspend *sus, int duration)
{
	if (!sus) return -1;
	sus->iWakeUp = 1;
	sus->iWakeDuration = duration > 0 ? duration : 1000;
	return 0;
}

int cm_suspend_status(CMSuspend *sus)
{
	if (!sus) return -1;
	return sus->iStatus;
}

int cm_suspend_check(CMSuspend *sus)
{
	if (!sus) return -1;
	if (!sus->iStart) return -2;
	unsigned long long now = cm_gettime_milli();
	if (now - sus->ullLastTime < (unsigned long long) sus->iDuration) {
        return 0;
	}
	if (sus->iStatus == 0) {
		sus->ullLastSuspend = now;
		sus->iStatus = 1;
		return 10;
	}
	if (sus->iWakeUp) {
        if (now - sus->ullLastSuspend > (unsigned long long) sus->iWakeDuration) {
            sus->iStatus = 0;
			sus->iStart = 0;
			return 20;
		}
	}
	return 11;
}

