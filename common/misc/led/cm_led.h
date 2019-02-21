/*********************************************************
 *
 * Filename: misc/led/cm_led.h
 *   Author: jmdvirus
 *   Create: 2019年01月02日 星期三 19时54分49秒
 *
 *********************************************************/

#include <stdio.h>

typedef struct _CMLedCtrl {
	char        szDev[128];
	FILE        *fd;

	char        cmd_on[144];
    char        cmd_off[144];
} CMLedCtrl;

int cm_led_ctrl_init(CMLedCtrl *lc, const char *dev);
int cm_led_ctrl_deinit(CMLedCtrl *lc);

int cm_led_ctrl_on(CMLedCtrl *lc);
int cm_led_ctrl_off(CMLedCtrl *lc);


