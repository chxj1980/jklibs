/*
 *===========================================================================
 *
 *          Name: demo_utils.c
 *        Create: 2015年10月20日 星期二 13时13分52秒
 *
 *   Discription: 
 *
 *        Author: yuwei.zhang
 *         Email: yuwei.zhang@besovideo.com
 *
 *===========================================================================
 */

#include <stdio.h>

#include "cm_utils.h"
#include "cm_logprint.h"

CMLogPrint logPrint;

int demo_read_file_data()
{
    const char *filename = "/tmp/readfiletest.data";
    char *data = NULL;
    int lenread = 0;
    int out = cm_read_file_data(filename, &data, &lenread);
    if (out < 0) {
        cmerror("read file data failed: %d", out);
        if (data) free(data);
        return -1;
    }
    cminfo("read file data out %d,%d of data", lenread, out);
    if (data) free(data);
    return 0;
}

int demo_read_file_data_more()
{
    char files[][256] = {
        "/tmp/kfcustom.log",
        "/tmp/kfmanager.log",
        "/tmp/dhcp.leases"
    };

    size_t cnts = sizeof(files)/sizeof(files[0]);
    size_t i;
    char *data = NULL;
    int   lendata = 0;
    for (i = 0; i < cnts; i++) {
        int out = cm_read_file_data(files[i], &data, &lendata);
        if (out < 0) {
            cmerror("read files failed. %d", out);
            if (data) free(data);
            return -2;
        }
        cminfo("Read down index [%d]", i);
    }
    cminfo("read file data out %d of data", lendata);
    if (data) free(data);
    return 0;
}

int demo_jk_time_string()
{
    const char *tStr = cm_time_string(0);
    cminfo("The string is %s", tStr);
    return 0;
}

int main()
{
    //demo_read_file_data();
    //demo_read_file_data_more();
    // demo_jk_time_string();
    return 0;
}


/*=============== End of file: demo_utils.c ==========================*/
