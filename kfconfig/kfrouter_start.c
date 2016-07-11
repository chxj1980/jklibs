/*
 *===========================================================================
 *
 *          Name: kfrouter_start.c
 *        Create: 2015年08月05日 星期三 11时19分24秒
 *
 *   Discription: 
 *
 *        Author: jmdvirus
 *         Email: jmdvirus@roamter.com
 *
 *===========================================================================
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "rt_print.h"
#include "kfconfig.h"

char dcFile[256] = "/usr/bin/kfconfig.bin";
char dstPath[128] = "/";

int main(int argc, char **args)
{
#if 0
    rtdebug("args number [%d]", argc);
    int i;
    for (i = 0; i < argc; i++) {
        rtdebug("[ %s ]", args[i]);
    }
#endif
    if (argc >= 2) {
        snprintf(dcFile, 256, "%s", args[1]);
    }
    if (argc >= 3) {
        snprintf(dstPath, 128, "%s", args[2]);
    }

    //rtdebug("dcfile [%s] dstPath [%s]", dcFile, dstPath);

    int ret = kfconfig_decompress(dcFile, dstPath);
    if (ret != 0) {
        return ret;
    }
    if (access("/tmp/kf/sscheck.sh", F_OK)) {
        // failed
        // file is not exist. copy kfconfig.bin.old to kfconfig.bin
        rtwarn("/tmp/kf nothing, do something!");
        char copystr[256] = {0};
        sprintf(copystr, "cp %s %s", "/usr/bin/kfconfig.bin.old", "/usr/bin/kfconfig.bin");
        if (system(copystr)) ;
        int ret = kfconfig_decompress(dcFile, dstPath);
        if (ret != 0) {
            return ret;
        }
    }

    return 0;
}


/*=============== End of file: kfrouter_start.c ==========================*/
