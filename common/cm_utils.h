/**
 *          File: cm_utils.h
 *
 *        Create: 2014年12月19日 星期五 14时13分45秒
 *
 *   Discription: 
 *
 *       Version: 1.0.0
 *
 *        Author: jmdvirus
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

#ifndef __CM_UTILS_H
#define __CM_UTILS_H

#include <time.h>
#include <stdlib.h>

#include <ctype.h>

// Base mem function.

#define cm_mem_malloc(size)    \
             malloc(size)
#define cm_mem_calloc(count, size)    \
             calloc(count, size)
#define cm_mem_realloc(t, size)     \
             realloc(t, size)
#define cm_mem_free(t)      \
             free(t);


enum {
    CM_WALLTIME_TYPE_NONE = 0,
    CM_WALLTIME_TYPE_ONCE = 1,
    CM_WALLTIME_TYPE_INTERVAL = 2,
};

enum {
    CM_WALLTIME_TIMETYPE_NONE = 0,
    CM_WALLTIME_TIMETYPE_EVERYDAY = 1,
    CM_WALLTIME_TIMETYPE_YEARDAY = 2,
    CM_WALLTIME_TIMETYPE_MONTHDAY = 3,
    CM_WALLTIME_TIMETYPE_WEEK = 4,
    CM_WALLTIME_TIMETYPE_ONCE = 5,
};

typedef struct {
    int        iYear;
    int        iMon;
    int        iDay;
    int        iHour;
    int        iMinute;
    int        iSecond;
} CMWallTime;

typedef struct {
    int         iType;         // once, interval (CM_WALLTIME_TYPE_*)
    int         iTimeType;     // each week, everyday, year day, month day. (CM_WALLTIME_TIMETYPE_*)
    int         iWeekDay;      // which day of week, 0-6 : sunday to monday
    CMWallTime  szStartTime;
    CMWallTime  szEndTime;
} CMWallTimeCondition;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Set time
 * @param t
 * @return 0
 */
int cm_walltime_set(CMWallTime *t, int year, int month, int day,
                    int hour, int minute, int second);

/**
 * If time in
 * @param cond
 * @return 1 - in, other no
 */
int cm_walltime_in(CMWallTimeCondition *cond);

// 0xff,0x88,0x9 --> 0xff 0x88 0x9 (in @data)
int cm_parse_data(unsigned char *data, char *string);

// ex: xx,xxx,xxxx
// parse from @string to find each @sign, with @maxlen
// save them to @save for max @arrsize
// return max found.
int cm_parse_data_string(char *string, char *save, int arrsize, int maxlen, const char sign);

// ex: d,s,2
// Parse from @string of @maxlen with @sign,
// save them to @data
// @data must enough long.
// return how many  be parsed.
int cm_parse_data_char(unsigned char *data, char *string, int maxlen, const char *sign);

// ex:d,s,2
// Parse from @string of @maxlen with @sign
// save them to @save.
// @save must enough long.
// return length be parsed.
int cm_parse_string_int(char *string, int *save, int maxlen, const char *sign);

// ex: (xxxx) -> xxxx
// remove parenthesis of @string, save it to @save.
int cm_clear_parenthesis(char *save, char *string);

// Like up, be change @string.
int cm_clear_parenthesis_self(char *string);


// Remote the last '\n' mark.
int cm_remove_last_break(char *args);

// clear @c from origin, save to save.
int cm_clear_string_char(char *origin, char *save, char c);

///////////////
// Take String to Path and Name
// ex: /Path/Filename
// Warn: you must sure the path and name has enough space.
int cm_seperate_filename(char *orig, char *path, char *name);

//////////////////////
// With Network
// Get the ip of @dn_or_ip, 
// @eth: device name (like eth,ppp1)
char *cm_get_ip(char *dn_or_ip, const char *eth);

int cm_get_ip_dev(const char *dev, char *ipaddr);

int cm_get_mac(char * mac, int len_limit, char *dev);    //返回值是实际写入char * mac的字符个数（不包括'\0')

// return 1: valid
//   other: not valid
int cm_check_mac_valid(const char *mac_addr);

int cm_generate_mac_with_char(const char *mac, char *save, char c);

// Get flow of pointed ip
// @interface (like eth,wlan ...)
int cm_get_flow(const char *interface, unsigned long long *recv,unsigned long long *send, unsigned long long *total);

// Convert int @value to char and save to @save,
// the return pointer to @save
// return NULL if fail
const char *cm_itoa(int value, char *save);

// convert string to value to @out
// return < 0 fail
//        == 0 success
int cm_atoi(const char *value, int *out);

// codec parse of sps pps sde
// Some may be contain sde info, we need ignore it.
int cm_codec_parse_pps(char *pps, int lenpps);
// @data ex: 00 00 00 01 [sps] 00 00 00 01 [pps]
/*
 * @data the data
 * @pps will save pps here
 * @sps will save sps here
 */
int cm_codec_parse(char *data, int len, char *pps, int *lenpps, char *sps, int *lensps);

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
int cm_read_file_data(const char *filename, char **data, int *len);

/*
 * @func: Generate time string with seconds.
 * Everycall will change the string value.
 * It is not thread safe and not reentrance.
 * @tm: If <= 0, it will get current time.
 */
const char *cm_time_string(time_t tm);

unsigned int cm_gettime_only_micro();

/**
 * return now time with microsecond
 */
unsigned long long cm_gettime_micro();

/**
 * random generate num's chars to @result
 * */
int cm_random_with_num_char(char *result, int num);
int cm_random_with_num_char_sym(char *result, int num);

/**
 * return now time with millisecond
 */
unsigned long long cm_gettime_milli();


int cm_format_time(char *save, int max, char *format);

int cm_format_time_t(char *save, int max, char *format, time_t t);

int cm_format_time_local(char *save, int max, char *format, struct tm *tm);

/*
 * @func: compare string with the maxlength.
 * @src, @dst: same with strcmp
 */
int kf_string_compare(const char *src, const char *dst);

int hexToByte(char *str, int ilen, char *save);

#include <stdint.h>
/*
 * @func: hex to binary
 */
uint32_t cm_hex2bin (void *bin, char hex[]);

void cm_bin2scr (void *bin, uint32_t len);

int cm_get_wifi_signal(const char *dev);

// filesystem
unsigned long long cm_get_ubi_size(const char *dev);

unsigned long long cm_get_ubi_available(const char *dev);

unsigned long long cm_get_ubi_free(const char *dev);

uint8_t cm_http_getfile(char *sUrl, char *sFileName);

unsigned short cm_litte_to_big(unsigned short v);

unsigned short cm_big_to_little(unsigned short v);

// from last to take string from split
// clear @save yourself,
// you must sure save big enough
int cm_take_out_last_string(const char *str, char split, char *save);

unsigned long cm_get_file_size(const char *filename);

int cm_wifi_signal_level(int signal);

#ifdef __cplusplus
}
#endif

#endif  // __CM_UTILS_H

/*=============== End of file: cm_utils.h =====================*/
