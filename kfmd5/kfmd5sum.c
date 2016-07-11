
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef KF_MBEDTLS
#include "kfmd5_mbedtls.h"
#else
#include "ngx_md5.h"
#endif

int main(int argc, char **args)
{
    if (argc < 2) {
        printf("Error: %s [string]\n", args[0]);
        return -1;
    }
    char *str = args[1];
#ifdef KF_MBEDTLS
    unsigned char result[16] = {0};
    kfmd5_mbedtls((unsigned char*)str, strlen(str), result);
#else
    u_char result[16] = {0};
    kfmd5_ngx((unsigned char *)str, strlen(str), result);
#endif
    int i;
    for (i = 0; i < 16; i++) {
         printf("%02x", result[i]);
    }
    return 0;
}
