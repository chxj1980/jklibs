/*
 *
 * 
 */

#include "jkbytes.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>

void HexStrToByte(const char* source, unsigned char* dest, int sourceLen)
{
    short i;
    unsigned char highByte, lowByte;

    for (i = 0; i < sourceLen; i += 2)
    {
        highByte = toupper(source[i]);
        lowByte  = toupper(source[i + 1]);

        if (highByte > 0x39)
            highByte -= 0x37;
        else
            highByte -= 0x30;

        if (lowByte > 0x39)
            lowByte -= 0x37;
        else
            lowByte -= 0x30;

        dest[i / 2] = (highByte << 4) | lowByte;
    }
    return ;
}

/* 字符串拆开，重新组装
 *  * eg: ab\xe8\x80\x83\xe5\x8b\xa4转换为
 *   *        ab鑰冨嫟(即ab考勤)
 *    */
void string_reerect(char *src, char *destBuf)
{
    int i = 0;
    int j = 0;

    while(src[j] != '\0')
    {
        if(strncmp(&src[j], "\\x", 2) != 0)
        {
            memcpy(&destBuf[i], &src[j], 1);
            i = i + 1;
            j = j + 1;
        }
        else
        {
            HexStrToByte(&src[j+2], &destBuf[i], 2);
            i = i+1;
            j = j+4;
        }
    }
    destBuf[i] = '\0';

    return ;
}


