/*
 *===========================================================================
 *
 *          Name: demo_md5.c
 *        Create: 2015年10月20日 星期二 19时24分08秒
 *
 *   Discription: 
 *
 *        Author: yuwei.zhang
 *         Email: yuwei.zhang@besovideo.com
 *
 *===========================================================================
 */

#include <stdio.h>
#include <string.h>

#include "ngx_md5.h"

int main()
{
    const char *data = "0.0.1\r\nmac\r\n1\r\n0\r\n1445339782\r\nab3w-be82f231-aa-cd1b";
    int lendata = strlen(data);

    unsigned char output[16] = {0};
    kfmd5_ngx((unsigned char*)data, lendata, output);
 
    int i;
    char sdata[34] = {0};
    for (i = 0; i < 16; i++) {
        printf("%02x", output[i]);
        char tmp[8] = {0};
        sprintf(tmp, "%02x", output[i]);
        strcat(sdata, tmp);
    }

    printf("\n");
    printf("the convert data: [%s]", sdata);


    return 0;
}


/*=============== End of file: demo_md5.c ==========================*/
