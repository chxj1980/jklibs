/**
 *          File: bvpu_utils.h
 *
 *        Create: 2014年12月19日 星期五 14时13分45秒
 *
 *   Discription: 
 *
 *       Version: 1.0.0
 *
 *        Author: yuwei.zhang@besovideo.com
 *
 *===================================================================
 */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) @ BesoVideo, 2014
 */

#ifndef __BVPU_UTILS_H
#define __BVPU_UTILS_H

#include <time.h>
#include <stdlib.h>

// Base mem function.

#define jk_mem_malloc(size)    \
             malloc(size)
#define jk_mem_calloc(count, size)    \
             calloc(count, size)
#define jk_mem_realloc(t, size)     \
             realloc(t, size)
#define jk_mem_free(t)      \
             free(t);

#define bvpu_mem_malloc(size)    \
             malloc(size)
#define bvpu_mem_calloc(count, size)    \
             calloc(count, size)
#define bvpu_mem_realloc(t, size)     \
             realloc(t, size)
#define bvpu_mem_free(t)      \
             free(t);

// 0xff,0x88,0x9 --> 0xff 0x88 0x9 (in @data)
int bvpu_parse_data(unsigned char *data, char *string);

// ex: xx,xxx,xxxx
// parse from @string to find each @sign, with @maxlen
// save them to @save for max @arrsize
// return max found.
int bvpu_parse_data_string(char *string, char *save, int arrsize, int maxlen, const char sign);

// ex: d,s,2
// Parse from @string of @maxlen with @sign,
// save them to @data
// @data must enough long.
// return how many  be parsed.
int bvpu_parse_data_char(unsigned char *data, char *string, int maxlen, const char *sign);

// ex:d,s,2
// Parse from @string of @maxlen with @sign
// save them to @save.
// @save must enough long.
// return length be parsed.
int bvpu_parse_string_int(char *string, int *save, int maxlen, const char *sign);

// ex: (xxxx) -> xxxx
// remove parenthesis of @string, save it to @save.
int bvpu_clear_parenthesis(char *save, char *string);

// Like up, be change @string.
int bvpu_clear_parenthesis_self(char *string);


// Remote the last '\n' mark.
int bvpu_remove_last_break(char *args);

///////////////
// Take String to Path and Name
// ex: /Path/Filename
// Warn: you must sure the path and name has enough space.
int bvpu_seperate_filename(char *orig, char *path, char *name);

//////////////////////
// With Network
// Get the ip of @dn_or_ip, 
// @eth: device name (like eth,ppp1)
char *bvpu_get_ip(char *dn_or_ip, const char *eth);

// Get flow of pointed ip
// @interface (like eth,wlan ...)
int bvpu_get_flow(const char *interface, unsigned long long *recv,unsigned long long *send, unsigned long long *total);

// Convert int @value to char and save to @save,
// the return pointer to @save
// return NULL if fail
const char *bvpu_itoa(int value, char *save);

// convert string to value to @out
// return < 0 fail
//        == 0 success
int bvpu_atoi(const char *value, int *out);

// codec parse of sps pps sde
// Some may be contain sde info, we need ignore it.
int bvpu_codec_parse_pps(char *pps, int lenpps);
// @data ex: 00 00 00 01 [sps] 00 00 00 01 [pps]
/*
 * @data the data
 * @pps will save pps here
 * @sps will save sps here
 */
int bvpu_codec_parse(char *data, int len, char *pps, int *lenpps, char *sps, int *lensps);

/*
 * @func: If the program with string @prog is running
 * @cnts: how many args to compare
 * @prog: string to compare.
 * @return: > 1 is runing. other not runing
 */
int is_program_running(int cnts, const char *prog[]);

/*
 * @func: Read of data of the file 
 * @filename: the file need to read.
 * @data: the position to save data.
 * @len: the length has read.
 * @return: > 0 success read length. -1 args error, -2, file open fail.
 * @warn: you must free @data, when you needn't it.
 */
int jk_read_file_data(const char *filename, char **data, int *len);

/*
 * @func: Generate time string with seconds.
 * Everycall will change the string value.
 * It is not thread safe and not reentrance.
 * @tm: If <= 0, it will get current time.
 */
const char *jk_time_string(time_t tm);

/*
 * @func: compare string with the maxlength.
 * @src, @dst: same with strcmp
 */
int kf_string_compare(const char *src, const char *dst);

#include <stdint.h>
/*
 * @func: hex to binary
 */
uint32_t rt_hex2bin (void *bin, char hex[]);

void rt_bin2scr (void *bin, uint32_t len);

/**************************************************************************
 * 函数名称：rt_files_return_clientmac_via_clientip
 * 函数功能：接收数据
 * 参数：    char *input_client_ip	该参数为要查询的客户端IP地址
 * 返回值：
 *	IP地址字符串		查询查询成功
 *      "fopen fail"            open files fail
 *      "find no result"        no resut find
 ***************************************************************************/
char *rt_files_return_clientmac_via_clientip(char *input_client_ip);//通过客户端的IP地址在DHCP文件中查询该客户端的MAC地址，并返回MAC地址


#endif  // __BVPU_UTILS_H

/*=============== End of file: bvpu_utils.h =====================*/
