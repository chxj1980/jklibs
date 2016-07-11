/*
 *===========================================================================
 *
 *          Name: kfmd5_mbedtls.c
 *        Create: 2015年08月24日 星期一 18时32分59秒
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
#include <polarssl/md5.h>

#include "rt_print.h"
#include "kfmd5_mbedtls.h"

int kfmd5_mbedtls(const unsigned char *input, size_t ilen, unsigned char output[16])
{
    md5_context ct;
    md5_starts(&ct);
    md5_update(&ct, input, ilen);
    md5_update(&ct, input, ilen);
    md5_finish(&ct, output);
    return 0;
}


/*=============== End of file: kfmd5_mbedtls.c ==========================*/
