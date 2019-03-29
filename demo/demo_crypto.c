/*
 *===========================================================================
 *
 *          Name: demo_crypto.c
 *        Create: 五 12/11 16:05:27 2015
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
#include <stdlib.h>

#include "rt_print.h"

#include "crypto/polaraes.h"
#include "crypto/base64.h"

int test_crypto_base64_en()
{
    unsigned char *src = (unsigned char *)"this is for test ! startit";
    unsigned char save[64] = {0};
    size_t dlen = 64;
    int ret = kf_base64_encode(save, &dlen, src, strlen((char*)src));
    if (ret < 0) {
        rterror("base 64 encode failed. %d", ret);
    }
    rtinfo("base 64 encode : %s", save);
    return ret;
}

int test_crypto_base64_de()
{
    unsigned char *src = (unsigned char *)"dGhpcyBpcyBmb3IgdGVzdCAhIHN0YXJ0aXQ=";
    unsigned char save[64] = {0};
    size_t dlen = 64;
    int ret = kf_base64_decode(save, &dlen, src, strlen((char*)src));
    if (ret < 0) {
        rterror("base 64 decode failed . %d", ret);
    }
    rtinfo("base 64 decode : %s", save);
    return 0;
}

#if 1
static const unsigned char aes_test_cfb128_iv[16] =
{
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
};
#else
static const unsigned char aes_test_cfb128_iv[16] =
        {
                0x00
        };
#endif

#include "bvpu_utils.h"

char* test_crypto_aes_en(const char *key, const char *cryptstring)
{
    if (!key || !cryptstring) return NULL;

    aes_context ctx;
    memset(&ctx, 0, sizeof(aes_context));

    char binkey[16] = {0};
    int outlen = rt_hex2bin(binkey, (char*)key);

    int ret = kf_aes_setkey_enc(&ctx, binkey, 128);
    if ( ret < 0 ) {
        rterror("aes set key enc failed %x", ret);
        return ret;
    }

    unsigned char iv[16] = {0};
    memcpy(iv, aes_test_cfb128_iv, 16);

    unsigned char inputdata[512] = {0};
    int ibinlen = strlen(cryptstring);
    strcpy(inputdata, cryptstring);

    unsigned char outputdata[512] = {0};
//    memcpy(outputdata, iv, 16);
    rtinfo("Start to crypt %d", ibinlen);
    size_t leniv = 0;
    ret = kf_aes_crypt_cfb128(&ctx, AES_ENCRYPT, ibinlen, &leniv, iv, inputdata, outputdata);
//    ret = kf_aes_crypt_cbc(&ctx, AES_ENCRYPT, ibinlen, iv, inputdata, outputdata);
    if ( ret < 0 ) {
        rterror("aes crypto failed %x", ret);
    }

    //unsigned char *src = (unsigned char *)"this is for test ! startit";
    static unsigned char save[512] = {0};
    size_t dlen = 512;
    ret = kf_base64_encode(save, &dlen, outputdata, strlen((char*)outputdata));
    if (ret < 0) {
        rterror("base 64 encode failed. %d", ret);
    }
    return save;
}

char * test_crypto_aes_de(const char *key, const char *cryptstring)
{
    aes_context ctx;
    memset(&ctx, 0, sizeof(aes_context));

    char binkey[16] = {0};
    int outlen = rt_hex2bin(binkey, (char*)key);
    int ret = kf_aes_setkey_dec(&ctx, binkey, 128);
    if ( ret < 0 ) {
        rterror("aes set key enc failed %x", ret);
        return ret;
    }
    unsigned char iv[16] = {0};
    memcpy(iv, aes_test_cfb128_iv, 16);

    unsigned char *src = (unsigned char *)cryptstring;
    size_t dlen = strlen(src);
    unsigned char *save = (unsigned char *)bvpu_mem_calloc(1, dlen+1);
    if (!save) return NULL;
    size_t lensave = dlen;

    ret = kf_base64_decode(save, &lensave, src, dlen);
    if (ret < 0) {
        rterror("base 64 decode failed . %d", ret);
        return NULL;
    }
    rtinfo("decode from base64 len: %d", lensave);

//    memcpy(iv, save, 16);
    unsigned char todec[512] = {0};
    memcpy(todec, save, lensave);

    rtinfo("Start to decrypt");
    static unsigned char outputdata[512] = {0};
    size_t leniv = 0;
    ret = kf_aes_crypt_cfb128(&ctx, AES_DECRYPT, strlen(todec), &leniv, iv, todec, outputdata);
//    ret = kf_aes_crypt_cbc(&ctx, AES_DECRYPT, strlen(todec), iv, todec, outputdata);
    if ( ret < 0 ) {
        rterror("aes crypto failed %x", ret);
    }
    if (save) {
        bvpu_mem_free(save);
    }

    return outputdata;
}

#include "crypto/aeslib.h"

int test_aeslib_encrypt(unsigned char *key, unsigned char *iv, char *plaintext, unsigned char *crypttext, int *lencrypt)
{

    int ret = encryptCBC(plaintext, strlen(plaintext), key, 128, iv);
    static unsigned char save[512] = {0};
    size_t dlen = 512;
    ret = kf_base64_encode(save, &dlen, plaintext, 64);
    if (ret < 0) {
        rterror("base 64 encode failed. %d", ret);
    }
    memcpy(crypttext, save, dlen);

    return ret > 0;
}

int test_aeslib_decrypt(unsigned char *key, unsigned char *iv, char *crypttext, int *lencrypt, char *plaintext)
{
    char save[512] = {0};
    size_t lensave = 512;
    int ret = kf_base64_decode(save, &lensave, crypttext, lencrypt);
    if (ret < 0) {
        rterror("base 64 decode failed . %d", ret);
        return NULL;
    }
//    rtinfo("decode result base 64: %s", save);
    ret = decryptCBC(save, strlen(save), key, 128, iv);
//    rtinfo("decrypt cbc result: %s", save);

    memcpy(plaintext, save, 64);
    return ret > 0;
}

int main()
{
    //kf_base64_self_test(1);
#if 0
    test_crypto_base64_en();
    test_crypto_base64_de();
#endif

#if 1
    const char *thekey = "32bitstringtofor";
    char *cryptstring = "This is a test for the data ''? fined why. okaes";
    char *encryptstr = test_crypto_aes_en(thekey, cryptstring);
    rtinfo("encryptstr: %s", encryptstr);

//    char *gotode = "AQIDBAUGBwgJEAoLDA0OD3T31Ojx+O0jdpP9Q9JLoDiVPE6EJTQIyFSY+ignJkUNeBfm0ZN01+EfmwkaS6y59Q==";
//    char *gotode = "YWv+OsFABP6NXfCC/AoVl+ulBSnuGeM+2ggMQAupjapazcMhlw0wB/lx2MW4BNVkHNRkW9M+UzFiz75vOTgB7A==";
//    char *gotode = "Z39hJaWazahck950dHSBtOVV83AaJkFsxJZi9UqQ7VFEYYu1Uf0aLPP/ioGUjX1X/k2z3hV9EOxlDMpAjCSGvg==";
    char *gotode = encryptstr;
    char *decryptstr = test_crypto_aes_de(thekey, gotode);
    rtinfo("decryptstr: %s", decryptstr);

#else
    const char *thekey = "32bitstringtofor";

    char *cryptstring = "This is a test for the data ''? fined why. okaes";
    unsigned char cdata[512] = {0};
    strcpy(cdata, cryptstring);
    unsigned char saveedata[512] = {0};
    int ret = test_aeslib_encrypt(thekey, aes_test_cfb128_iv, cdata, saveedata, NULL);
    rtinfo("encode result: %s", saveedata);

//    char *gotode = "AQIDBAUGBwgJEAoLDA0OD3T31Ojx+O0jdpP9Q9JLoDiVPE6EJTQIyFSY+ignJkUNeBfm0ZN01+EfmwkaS6y59Q==";
//    char *gotode = saveedata;
    char *gotode = "YWv+OsFABP6NXfCC/AoVl+ulBSnuGeM+2ggMQAupjapazcMhlw0wB/lx2MW4BNVkHNRkW9M+UzFiz75vOTgB7A==";
    unsigned char data[512] = {0};
    strcpy(data, gotode);
    unsigned char savedata[512] = {0};
    ret = test_aeslib_decrypt(thekey, aes_test_cfb128_iv, data, strlen(gotode), savedata);
    rtinfo("decode restult: %s", savedata);

#endif

    return 0;
}


/*=============== End of file: demo_crypto.c ==========================*/
