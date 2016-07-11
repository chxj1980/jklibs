/*
 *===========================================================================
 *
 *          Name: kfconfig.c
 *        Create: 2015年08月05日 星期三 10时51分24秒
 *
 *   Discription: 
 *
 *        Author: jmdvirus
 *         Email: jmdvirus@roamter.com
 *
 *===========================================================================
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "rt_print.h"

/*
 * Decompress files
 * @dcFile: The file compressed.
 * @dstPath: Decompress to there.
 */
int kfconfig_decompress(const char *dcFile, const char *dstPath)
{
    int ret = 0;

    char *pass = (char*)"ard;!se3k2r3ra21a8?321:1adf,2a1aewf8s,aa1a";
    char passed[64] = {0};
    char *p1 = pass+10;
    char *p2 = pass+(strlen(pass) -12);
    strncpy(passed, p1, p2-p1);
    // to decode.
    char decode[256] = {0};

    char decompress[128] = {0};

    if (access(dcFile, F_OK)) {
        rterror("File [ %s ] is not exist, please check!", dcFile);
        ret = -1;
        goto errout;
    }

    //rtdebug("passed [%s]", passed);
    //mbedtls_aescrypt2 1 $infile $infile.tmp string:$pass
    sprintf(decode, "%s 1 %s %s string:%s", "/usr/bin/aescrypt2", dcFile,  "/tmp/kfrouter.tmp", passed);
    if (system(decode) == -1) {
        rterror(" exec [ %s ] failed. ", decode);
        ret = -2;
        goto errout;
    }

    // to decompress
    // tar zxf $infile.tmp
    sprintf(decompress, "tar zxf /tmp/kfrouter.tmp -C %s", dstPath);
    if (system(decompress) == -1) {
        rterror(" exec [ %s ] failed. ", decompress);
        ret = -3;
        goto errout;
    }

errout:

    // Remove the tmp file
    remove("/tmp/kfrouter.tmp");
    return ret;
}

/*=============== End of file: kfconfig.c ==========================*/
