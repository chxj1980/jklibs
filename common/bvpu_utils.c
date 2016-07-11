/**
 *          File: bvpu_utils.c
 *
 *        Create: 2014年12月19日 星期五 14时13分24秒
 *
 *   Discription: 
 *
 *        Author: yuwei.zhang
 *         Email: yuwei.zhang@besovideo.com
 *
 *===========================================================================
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 
// Common functions
//
#include "bvpu_utils.h"

int bvpu_parse_data(unsigned char *data, char *string)
{
    char p[128];
    char *str;
    char *token;
    int  i = 0;
    int val = 0;

    strncpy(p, string, strlen(string));

    for (str = p;;str = NULL)
    {
        token = strtok(str, ",");
        if (token == NULL) {
            break;
        } else {
            sscanf(token, "%02x", &val);
            data[i++] = val;
        }
    }
    
    return i+1;
}

int bvpu_parse_data_string(char *string, char *save, int arrsize, int maxlen, const char sign)
{
    if (string == NULL || save == NULL || arrsize < 0 || maxlen < 0) return -1;
    int      i = 0, j = 0;
    int      counts = 0;
    char    *pos = index(string, sign);
    if (!pos) return -2;
    while((pos = index(pos, sign))) {
        pos++;
        counts++;
    };
    counts += 1;   // we find counts

    pos = string;
    memset(save, 0, arrsize*maxlen);
    for (i = 0; i < maxlen; i++) {
        if (i >= counts) break;    // only deal we find counts
        char *p = save+i*(arrsize);
        for (j = 0; j < arrsize;j++) {
            if (j == arrsize-1 || *pos == sign || *pos == '\0') { 
                *(p+j) = '\0'; 
                if ((pos = index(pos, sign))) pos++;
                break;
            }
            *(p+j) = *pos++;
        }   
    }
    return i;    // return what we realy deal, maybe not maxlen
}

int bvpu_parse_data_char(unsigned char *data, char *string, int maxlen, const char *sign)
{
    char p[128];
    char *str;
    char *token;
    int  i = 0;
    char val;

    strncpy(p, string, strlen(string));

    for (str = p;;str = NULL)
    {
        if (i >= maxlen) break;
        token = strtok(str, sign);
        if (token == NULL) {
            break;
        } else {
            sscanf(token, "%c", &val);
            data[i++] = val;
        }
    }
    
    return i;
}

int bvpu_parse_string_int(char *string, int *save, int maxlen, const char *sign)
{
    char  p[128];
    char *str;
    char *token;
    int   i = 0;
    int   val = 0;

    strncpy(p, string, strlen(string));

    for (str = p;;str = NULL)
    {
        if (i >= maxlen) break;
        token = strtok(str, sign);
        if (token == NULL) {
            break;
        } else {
            sscanf(token, "%d", &val);
            save[i++] = val;
        }
    }
    
    return i;
}

#if 0
// (xx,xx);(xx,xx)
// We don't control if great then imageSize max size
int bvpu_parse_string_imagerects(char *string, BVPU_SDK_ImageSize *imageSize, const char *sign)
{
    if (!string || !imageSize) return -1;

    BVPU_SDK_ImageSize *inImage = imageSize;

    char tmp[512] = {0};
    sprintf(tmp, "%s", string);
    char *p = tmp;
    char *str = NULL;
    char *token = NULL;
    int i = -1;
    for (str = p; ; str = NULL) {
        token = strtok(str, sign);
        if (token == NULL) {
            break;
        } else {
            sscanf(token, "(%d,%d)", &inImage->iWidth, &inImage->iHeight);
            inImage++;   // goto next
            i++;
        }
    }
   
    return i+1;
}
#endif

int bvpu_clear_parenthesis(char *save, char *string)
{
    if (save == NULL || string == NULL) return -1;

    int    len = strlen(string);
    if (string[0] != '(' && string[len-1] != ')') return -2;

    int    i = 0;
    int    j = 1;
    if (string[0] != '(') j = 0;
    for (i = 0; i < len; i++) {
        if (string[i] == ')') break;
        save[i] = string[j++];
    }

    return 0;
}
int bvpu_clear_parenthesis_self(char *string)
{
    if (string == NULL) return -1;
    
    char  save[1024] = {0};
    bvpu_clear_parenthesis(save, string);
    return sprintf(string, "%s", save);
}

#if 0
int bvpu_in_walltime(BVPU_SDK_WallTime *start, BVPU_SDK_WallTime *end, struct tm *now)
{
    if (!start || !end || !now) return BVPU_SDK_RESULT_E_INVALIDPARAM;
#if 0
    BVINFO("[%d:%d:%d-%d:%d:%d, %d:%d:%d]\n", start->iYear, start->iMonth, start->iDay,
              end->iYear, end->iMonth, end->iDay, 
              now->tm_year, now->tm_mon, now->tm_mday);
#endif

    // now year is outside of start and end year
    int now_year = now->tm_year + 1900;
    int now_mon = now->tm_mon + 1;
    if (now_year > end->iYear || now_year < start->iYear) {
        return -1;
    }

    // Others is valid year, judge month
    // 1. now year == start year, check if month and day valid
    // 2. now year == end year, check if month and day valid
    // 3. maybe now year == start year == end year, so need check
    //    start year and end year
    if (now_year == start->iYear) {
        // 1. cur mon < mon is fail
        // 2. cur mon > mon is valid
        // 3. cur mon == mon, need to check day
        if (now_mon < start->iMonth) return -1;
        if (now_mon == start->iMonth) {
            if (now->tm_mday < start->iDay) return -1;
        }
    } 

    if (now_year == end->iYear) {
        // same with upper rule
        if (now_mon > end->iMonth) return -1;
        if (now_mon == end->iMonth) {
            if (now->tm_mday > end->iDay) return -1;
        }
    }
 
    // if year, month, day check valid
    // next to check time
    // check time only when the start and end year,month,day are same
#if 0
    if (start->iYear == end->iYear && start->iMonth == end->iMonth 
           && start->iDay == end->iDay) {
BVINFO("%d,%d,%d-%d,%d,%d---%d,%d,%d-%d,%d,%d-- now %d,%d,%d,%d,%d,%d\n",
        start->iYear, start->iMonth, start->iDay, start->iHour, start->iMinute, start->iSecond,
        end->iYear, end->iMonth, end->iDay, end->iHour, end->iMinute, end->iSecond,
        now->tm_year, now->tm_mon, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
#endif
        
    time_t starttime = start->iHour * 3600 + start->iMinute * 60 + start->iSecond;
    time_t endtime = end->iHour * 3600 + end->iMinute * 60 + end->iSecond;
    time_t nowtime = now->tm_hour * 3600 + now->tm_min * 60 + now->tm_sec;
    // The same day, check time
    if (start->iYear == end->iYear && start->iMonth == end->iMonth &&
           start->iDay == end->iDay) {
        if (nowtime > endtime || nowtime < starttime) return -1;
    // The same with start day, check start time
    } else if (start->iYear == now_year && start->iMonth == now_mon
                  && start->iDay == now->tm_mday) {
        if (nowtime < starttime) return -1;
    // The same with end day, check end time
    } else if (end->iYear == now_year && end->iMonth == now_mon
                  && end->iDay == now->tm_mday) {
        if (nowtime > endtime) return -1;
    }
     
#if 0
    }
#endif

    return 0;
}

// 
// Check if now time in daytimeslice
int bvpu_in_daytimeslice(BVPU_SDK_DayTimeSlice *slice, struct tm *now)
{
    if (!slice || !now) return BVPU_SDK_RESULT_E_INVALIDPARAM;

    time_t nowsec = now->tm_hour * 3600 + now->tm_min * 60 + now->tm_sec;
    int j;
    // I just compare today depends on week day
    int today = now->tm_wday;    // what the week today
    for (j = 0; j < BVPU_SDK_MAX_DAYTIMESLICE_COUNT; j++) {
        //BV_DayTimeSlice *inSlice = &slice[i][j];
        // Here scanf day time slice * today begin from slice
        // And then is the read data
        BVPU_SDK_DayTimeSlice *inSlice = slice+today*BVPU_SDK_MAX_DAYTIMESLICE_COUNT+j;
        if (inSlice) {
#if 0
            BVINFO("compare time [%d][%d] [%d:%d:%d-%d:%d:%d]\n",
                  today, j, inSlice->cHourBegin, inSlice->cMinuteBegin, inSlice->cSecondBegin,
                  inSlice->cHourEnd, inSlice->cMinuteEnd, inSlice->cSecondEnd);
#endif
            time_t start = inSlice->cHourBegin * 3600 + 
                   inSlice->cMinuteBegin * 60 + inSlice->cSecondBegin;
            time_t end = inSlice->cHourEnd * 3600 + 
                   inSlice->cMinuteEnd * 60 + inSlice->cSecondEnd;
            // Find one, ignore others
            // If start and end are 0 means don't snapshots any
            if (start == end && end == 0) return -1;
            if (nowsec >= start && nowsec <= end) {
                return 0;
            }
        }
    }

    return -1;
}
#endif

int bvpu_remove_last_break(char *args)
{
    if (args == NULL) return -1;

    int  len = strlen(args);
    if (args[len-1] == '\n') 
        args[len-1] = '\0';

    return 0;
}

/////////////////////////////////////////////////////////////////////////////////
///////////////   Network
/////////////////////////////////////////////////////////////////////////////////
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#define h_addr h_addr_list[0]
char *bvpu_get_ip(char *dn_or_ip, const char *eth)
{
   struct hostent *host;
   struct ifreq req;
   int sock;
   if (dn_or_ip == NULL) return NULL;
   if (strcmp(dn_or_ip, "localhost") == 0) 
   {
	  sock = socket(AF_INET, SOCK_DGRAM, 0);
	  strncpy(req.ifr_name, eth, IFNAMSIZ);
	  if ( ioctl(sock, SIOCGIFADDR, &req) < 0 ) 
	  {
		 printf("ioctl error: %s\n", strerror (errno));
		 return NULL;
	  }
	  dn_or_ip = (char *)inet_ntoa(*(struct in_addr *) &((struct sockaddr_in *) &req.ifr_addr)->sin_addr);
	  shutdown(sock, 2);
	  close(sock);
   } 
   else 
   {
	  host = gethostbyname(dn_or_ip);
	  if (host == NULL) 
	  {
		 return NULL;
	  }
	  dn_or_ip = (char *)inet_ntoa(*(struct in_addr *)(host->h_addr));
   }
   return dn_or_ip;
}

int bvpu_get_flow(const char *interface, unsigned long long *recv,unsigned long long *send, unsigned long long *total)
{
    int ret = 0;
    unsigned int l1,l2;
    FILE *fp = fopen("/proc/net/dev","r");
    if(!fp) {
        perror("fail to open /proc/net/dev");
        if(recv)  *recv  = 0; 
        if(send)  *send  = 0; 
        if(total) *total = 0; 
        return -1; 
    }   
    char buf[512];
    while(fgets(buf,512,fp)) {
        if(strstr(buf,interface)) {
            char *ptr;
            if((ptr = strchr(buf, ':')) == NULL) {
                ret = -1; 
                goto out;
            }   
            sscanf(++ptr,"%d",&l1);
            sscanf(++ptr,"%*d%*d%*d%*d%*d%*d%*d%*d%d",&l2);
            if(recv)  *recv  = l1; 
            if(send)  *send  = l2; 
            if(total) *total = l1 + l2; 
            goto out;
        }   
    }   
out:
    if (fp) { fclose(fp); fp = NULL; }
    return ret;
}

///////////////
// Take String to Path and Name
// ex: /Path/Filename
// Warn: you must sure the path and name has enough space.
int bvpu_seperate_filename(char *orig, char *path, char *name)
{
    if (!orig) return -1;

    char *p = rindex(orig, '/');
    // Has no path. Just save to name.
    if (!p) {
        if (name) {
            sprintf(name, "%s", orig);
        }
        return 0;
    } else {
        if (path) {
            int len = p - orig;
            strncpy(path, orig, len);
            if (path[len] != '\0')
			    path[len] = '\0';
        }
        if (name) {
            // The p is '/'
            sprintf(name, "%s", p+1);
        }
    }

    return 0;
}

#include "math.h"
// Convert int @value to char and save to @save,
// the return pointer to @save
// return NULL if fail
const char *bvpu_itoa(int value, char *save)
{
#if 0
#ifdef MIPS
    return NULL;
#else
    if (isnan(value)) return NULL;

    sprintf(save, "%d", value);
    return save;
#endif
#else
    return NULL;
#endif
}

// convert string to value to @out
// return < 0 fail
//        == 0 success
int bvpu_atoi(const char *value, int *out)
{
    int ret = sscanf(value, "%d", out);
    if (ret != 1) {
        return -1;
    }
    return 0;
}

///////////
// media operation
//

// codec parse of sps pps sde
// Some may be contain sde info, we need ignore it.
int bvpu_codec_parse_pps(char *pps, int lenpps)
{
    char *p = pps;  // remember the start position, use for cacultate the length.
    char *pn = pps;
    int donelen = 0;
    int lenreadpps = 0;
    while (1) {
        pn++;  // The first is 0x 00 00 00 01, skip it.
        if (donelen +3 >= lenpps) break;
        // find next 0x 00 00 00 01
        if (*pn == 0x0 && *(pn+1) == 0x0 && *(pn+2) == 0x0 && *(pn+3) == 0x01) {
            char *now = pn;  // remeber the address.
            lenreadpps = pn - p;
            memmove(pps, p, lenreadpps);
            break;
        }
        pn++;
        donelen++;
    }
    return lenreadpps;
}

// @data ex: 00 00 00 01 [sps] 00 00 00 01 [pps]
/*
 * @data the data
 * @pps will save pps here
 * @sps will save sps here
 */
int bvpu_codec_parse(char *data, int len, char *pps, int *lenpps, char *sps, int *lensps)
{
    if (len <= 0 || !lensps || !lenpps || !sps || !pps || !data) return -1;
    char *p = data;
    char *ps = data; // sps start position
    int donelen = 0;
    while (1) {
        if (donelen+3 >= len) break;
        if (*p == 0x0 && *(p+1) == 0x0 && *(p+2) == 0x0 && *(p+3) == 0x01) {
            ps = p;   // remember here.
        }
        int bok = 0;
        char *pn = p+4;
        char *pns = pn;  // the pps start position
        while (1) {
            if (donelen+3 >= len) break;
            if (*pn == 0x0 && *(pn+1) == 0x0 && *(pn+2) == 0x0 && *(pn+3) == 0x01) {
                pns = pn;   // remember here.
                // Find it, now save pps and sps to the args.
                int lenout = pns - ps;
                memcpy(sps, ps, lenout);
                if (lensps) *lensps = lenout;
                lenout = len - *lensps;
                memcpy(pps, pns, lenout);
                if (lenpps) *lenpps = lenout;
                bok = 1;
                break;
            }
            pn++;
            donelen++;
        }
        if (bok) break;
        p++;
        donelen ++;
    }
    *lenpps = bvpu_codec_parse_pps(pps, *lenpps);
    return 0;
}

// TODO: check it validation
int hexToByte(char *str, int ilen, char *save) {
    int count=0;
    char *s = str;
    for(s = str; ilen > 0; s+=2)
    {
        if(*s>='0' && *s<='9') 
            save[count]=*s-'0';
        else if(*s>='A'&& *s<='F')
            save[count]=*s-'A'+10;
        if(strlen(s)>1)
        {
            save[count]*=16;
            if(*(s+1)>='0' && *(s+1)<='9') save[count++]+=(*(s+1)-'0');
            if(*(s+1)>='A'&& *(s+1)<='F') save[count++]+=(*(s+1)-'A'+10);
        }
    }
    return 0;
}

#include <dirent.h>
#include <string.h>

int is_program_running(int cnts, const char *prog[]) {
    struct dirent *d = NULL;
    DIR *dir = opendir("/proc");
    if (!dir) { return -1; }

    int ret = 0;
    while((d = readdir(dir)) != NULL) {
        char cmdname[64] = {0};
        if (d->d_type != DT_DIR) continue;
        sprintf(cmdname, "/proc/%s/cmdline", d->d_name);
        if (access(cmdname, R_OK) == 0) {
            FILE *f = fopen(cmdname, "r");
            if (f) {
                char data[1024] = {0};
                //int n = fread(data, 1, 64, f);
                int n = 0;
                char tdata[1024] = {0};
                int nn = fread(tdata, 1, 1024, f);
                int i;
                char *pt = tdata;
                char *p = data;
                for (i = 0; i < nn; i++) {
                    if (*pt != '\0') {
                        *p++ = *pt++;
                    } else {
                        pt++;
                    }
                    n++;
                }
                fclose(f);
                //printf("cmdline: %s, %s, %d\n", data, prog[1], n);
                if (n > 0) {
                    // It's program self.
                    if (strncmp(data, prog[0], strlen(prog[0])) == 0) {
                        break;
                    }
                    int tcnts = 0;
                    for (i = 0; i < cnts; i++) {
                        if (strstr((char*)data, prog[i+1]) == NULL) break;
                        tcnts++;
                    }
                    if (tcnts == cnts) {
                        //printf("I find you. %s\n", prog[1]);
                        ret = 1;
                        break;
                    }
                }
            }
        }
    }
    
    return ret;
}

int jk_read_file_data(const char *filename, char **data, int *len) {
    if (!filename || !data) return -1;
    FILE *f = fopen(filename, "r");
    if (!f) return -2;

    int lenread = *len;
    int leneach = 1024;
    int err = 0;
    while (!feof(f)) {
        *data = (char*)jk_mem_realloc(*data, lenread+leneach+1);
        if (!*data) { err = -11; break;}
        int out = fread(*data+lenread, 1, leneach, f);
        if (out <= 0) { err = -12; break;}
        lenread += out;
    }
    if (len) *len = lenread;

    fclose(f);
    return err != 0 ? err : lenread;
}

const char *jk_time_string(time_t tm)
{
    static char tStr[16] = {0};
    time_t t = tm;
    if (tm <= 0) {
        t = time(NULL);
    }
    sprintf(tStr, "%ld", t);
    return tStr;
}

int kf_string_compare(const char *src, const char *dst)
{
    if (!src || !dst) return -1;

    size_t slen = strlen(src);
    size_t dlen = strlen(dst);
    size_t clen = slen > dlen ? slen : dlen;
    return strncasecmp(src, dst, clen);
}

int _rt_isxdigit (int c)
{
  return (c >= '0' && c <= '9') || 
         (c >= 'a' && c <= 'f') ||
         (c >= 'A' && c <= 'F');
}

int _rt_isprint (int c)
{
  return 1;
}

uint32_t rt_hex2bin (void *bin, char hex[]) 
{
    uint32_t len, i;
    uint32_t x;
    uint8_t *p=(uint8_t*)bin;
    
    len = strlen (hex);
    
    if ((len & 1) != 0) {
        return 0; 
    }
    
    for (i=0; i<len; i++) {
        if (_rt_isxdigit((int)hex[i]) == 0) {
            return 0; 
        }
    }
    
    for (i=0; i<len / 2; i++) {
        sscanf (&hex[i * 2], "%2x", &x);
        p[i] = (uint8_t)x;
    } 
    return len / 2;
} 

void rt_bin2scr (void *bin, uint32_t len) 
{
  uint32_t i, ofs;
  uint8_t c;
  uint8_t *mem=(uint8_t*)bin;
  printf("\n");
  
  for (ofs=0; ofs<len; ofs+=16) 
  {
    printf ("\n%08X", ofs);
    for (i=0; i<16 && ofs+i < len; i++) {
        printf (" %02x", mem[ofs + i]);
    }
    while (i++ < 16) {
      printf ("   ");
    }
    printf ("    ");

    for (i=0; i<16 && ofs+i < len; i++) {
        c=mem[ofs+i];
        printf ("%c", (c=='\t' || !_rt_isprint (c)) ? '.' : c);
    }
  }
}

#include<string.h>
#include"rt_print.h"
//#include"rasp/kfrasp.h"
//KFConnectItemInfo device_info;//该结构体用于在rt_files_return_clientmac_via_clientip函数当中返回客户端和本机mac相关信息

char *file_path = "/home/pig/KF/kfconfig/common/dhcp.leases";//如果文件位置发生变动，请在此处修改//这种容易发生改动的变量最好定义为全局变量

/////////////////////////////////////////////////////////////////////////////////
//函数名称：int rt_files_return_clientmac_via_clientip
//函数功能：从dhcp.leases中查找IP地址所对应的设备信息包括客
//          户端mac地址客户端名称，路由器mac地址等
//参数：    char *input_client_ip	该参数为要查询的客户端IP地址
//返回值：
//           0  查询查询成功
//          -1  打开文件失败
//          -2  没有查找到IP地址所对应的相关信息）
/////////////////////////////////////////////////////////////////////////////////
int  rt_files_return_deviceinfo_via_clientip(char *input_client_ip)
//该函数执行完毕之后device_info结构体中将返回对应的数据
{
/*
    FILE *fp = NULL;//定义文件类型指针
    if((fp = fopen(file_path,"r")) == NULL)//以只读方式打开文件
    {
        rterror("open files error!\n");
        return -1;
    }
    while (!feof(fp))
    {
        char time_sign[64] = {0};
        char client_mac[64] = {0};
        char client_ip[64] = {0};
        char client_name[256] = {0};
        char router_mac[64] = {0};
        fscanf(fp, "%s", time_sign);//遇到空格时结束
        fscanf(fp, "%s", client_mac);
        fscanf(fp, "%s", client_ip);
        fscanf(fp, "%s", client_name);
        fscanf(fp, "%s", router_mac);
        //printf("%d %s %s\n", NumData, MACData, IPData);
        if(strcmp(client_ip, input_client_ip) == 0)
        {
            strcpy(device_info.szAddr, client_ip);
            //device_info.szAddr = client_ip;
            strcpy(device_info.szDeviceMac, client_mac);
            strcpy(device_info.szName, client_name);
            strcpy(device_info.szRouterMac, router_mac);
            fclose(fp);          

            device_info.szAddr = client_ip;
            //device_info.szAddr = client_ip;
            device_info.szDeviceMac = client_mac;
            strcpy(device_info.szName = client_name;
            strcpy(device_info.szRouterMac = router_mac;
            fclose(fp);        
            return 0;   

      //  }
    }
*/
            
      printf("a\n");
    FILE *fp = NULL;//定义文件类型指针
    if((fp = fopen(file_path,"r")) == NULL)//以只读方式打开文件
    {
        printf("open files error!\n");
        return -1;
    }
    printf("b\n");
    while (!feof(fp))
    {
        char time_sign[64] = {0};
        char client_mac[64] = {0};
        char client_ip[64] = {0};
        char client_name[256] = {0};
        char router_mac[64] = {0};
        printf("bb\n");
    
        //printf("cccccccccc\n");
        fscanf(fp, "%s", time_sign);//遇到空格时结束
        printf("timesign=%s\n",time_sign);
        
        fscanf(fp, "%s", client_mac);
        printf("client_mac=%s\n",client_mac);
        
        fscanf(fp, "%s", client_ip);
        printf("client_ip=%s\n",client_ip);
        
        fscanf(fp, "%s", client_name);
        printf("client_name=%s\n",client_name);
        
        fscanf(fp, "%s", router_mac);
        printf("router_mac=%s\n",router_mac);
        printf("bbbbb\n");
        //printf("%d %s %s\n", NumData, MACData, IPData);
        if(strcmp(client_ip, input_client_ip) == 0)
        {
            printf("client_ip is %s\n",client_ip);
            printf("client_mac is %s\n",client_mac);
            printf("client_name is %s\n",client_name);
            printf("router_mac is %s\n",router_mac);
            printf("c\n");
            return 0;             
        }
        printf("bbb\n");
     }
    printf("d\n");
    return -2;
}






/*=============== End of file: bvpu_utils.c ==========================*/
