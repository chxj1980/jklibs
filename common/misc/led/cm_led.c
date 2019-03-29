/*********************************************************
 *
 * Filename: misc/led/cm_led.c
 *   Author: jmdvirus
 *   Create: 2019年01月02日 星期三 19时57分24秒
 *
 *********************************************************/

#include <stdio.h>
#include <string.h>

#include "cm_led.h"
#include "cm/cm_utils.h"
#include "cm_logprint.h"

int cm_led_ctrl_init(CMLedCtrl *lc, const char *dev)
{
	if (!lc || !dev) return -1;
	memset(lc, 0, sizeof(*lc));
	cm_strncpy(lc->szDev, dev, sizeof(lc->szDev));
	sprintf(lc->cmd_on, "echo 1 > %s", lc->szDev);
	sprintf(lc->cmd_off, "echo 0 > %s", lc->szDev);
#if 0
	lc->fd = fopen(lc->szDev, "w");
	if (lc->fd < 0) {
		return -2;
	}
#endif
	return 0;
}

int cm_led_ctrl_deinit(CMLedCtrl *lc)
{
#if 0
	if (lc->fd) {
		fclose(lc->fd);
	}
#endif
	memset(lc, 0, sizeof(*lc));
	return 0;
}

int cm_led_ctrl_on(CMLedCtrl *lc)
{
	if (lc->szDev[0] == '\0') return -2;
	cmdebug("led ctrl set normal on of device [%s]\n", lc->szDev);
	int ret = 0;
#if 0
	int ret = fwrite("1", 1, 1, lc->fd);
#else 
	//int ret = cm_write_file_data(lc->szDev, "1", 1);
	system(lc->cmd_on);
#endif
	return ret;
}

int cm_led_ctrl_off(CMLedCtrl *lc)
{
	if (lc->szDev[0] == '\0') return -2;
	cmdebug("led ctrl set normal off of device [%s]\n", lc->szDev);
	int ret = 0;
#if 0
	int ret = fwrite("0", 1, 1, lc->fd);
#else
	//int ret = cm_write_file_data(lc->szDev, "0", 1);
	system(lc->cmd_off);
#endif
	return ret;
}

