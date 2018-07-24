/**
 *          File: cm_utils.c
 *
 *        Create: 2014年12月19日 星期五 14时13分24秒
 *
 *   Discription: 
 *
 *        Author: jmdvirus
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
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// 
// Common functions
//
#include "cm_utils.h"

#ifndef USE_LIBCURL
#define USE_LIBCURL 1
#endif

#if (USE_LIBCURL == 1)
#include <curl/curl.h>
#endif

int cm_walltime_set(CMWallTime *t, int year, int month, int day,
                    int hour, int minute, int second)
{
    t->iYear = year;
    t->iMon = month;
    t->iDay = day;
    t->iHour = hour;
    t->iMinute = minute;
    t->iSecond = second;
    return 0;
}

int cm_walltime_in(CMWallTimeCondition *cond)
{

    time_t now = time(NULL);
    struct tm *tm = localtime(&now);

    char starttime[64] = {0};
    char endtime[64] = {0};
    char nowtime[64] = {0};
    char nowd[64] = {0};
    sprintf(nowd, "%04d%02d%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
    sprintf(nowtime, "%04d%02d%02d%02d%02d%02d", tm->tm_year + 1900, tm->tm_mon + 1,
    tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
    char nowt[64] = {0};
    sprintf(nowt, "%02d%02d%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);

    switch (cond->iTimeType) {
        case CM_WALLTIME_TIMETYPE_EVERYDAY:
        {
            char startd[64] = {0};
            sprintf(startd, "%04d%02d%02d", cond->szStartTime.iYear, cond->szStartTime.iMon,
                    cond->szStartTime.iDay);
            char endd[64] = {0};
            sprintf(endd, "%04d%02d%02d", cond->szEndTime.iYear, cond->szEndTime.iMon,
                    cond->szEndTime.iDay);

            if (strcmp(startd, nowd) <= 0 && strcmp(nowd, endd) <= 0) {
                sprintf(starttime, "%02d%02d%02d",
                        cond->szStartTime.iHour, cond->szStartTime.iMinute,
                        cond->szStartTime.iSecond);

                sprintf(endtime, "%02d%02d%02d",
                        cond->szEndTime.iHour, cond->szEndTime.iMinute, cond->szEndTime.iSecond);
                if (strcmp(starttime, nowt) <= 0 && strcmp(nowt, endtime) <= 0) {
                    return 1;
                }
            }
        }
            break;
        case CM_WALLTIME_TIMETYPE_MONTHDAY:
        {
            if (cond->szStartTime.iYear <= tm->tm_year &&
                    cond->szEndTime.iYear >= tm->tm_year &&
                    cond->szStartTime.iMon <= tm->tm_mon &&
                    cond->szEndTime.iMon >= tm->tm_mon &&
                    cond->szStartTime.iDay == tm->tm_mday) {
                sprintf(starttime, "%02d%02d%02d", cond->szStartTime.iHour, cond->szStartTime.iMinute,
                        cond->szStartTime.iSecond);
                sprintf(endtime, "%02d%02d%02d", cond->szEndTime.iHour, cond->szEndTime.iMinute,
                        cond->szEndTime.iSecond);
                if (strcmp(starttime, nowt) <= 0 && strcmp(nowt, endtime) <= 0) {
                    return 1;
                }
            }
        }
            break;
        case CM_WALLTIME_TIMETYPE_WEEK:
        {
            int nowweek = tm->tm_wday;
            if (nowweek != cond->iWeekDay) return -1;
            char startd[64] = {0};
            sprintf(startd, "%04d%02d%02d", cond->szStartTime.iYear, cond->szStartTime.iMon,
                    cond->szStartTime.iDay);
            char endd[64] = {0};
            sprintf(endd, "%04d%02d%02d", cond->szEndTime.iYear, cond->szEndTime.iMon,
                    cond->szEndTime.iDay);

            if (strcmp(startd, nowd) <= 0 && strcmp(nowd, endd) <= 0) {
                sprintf(starttime, "%02d%02d%02d", cond->szStartTime.iHour, cond->szStartTime.iMinute,
                        cond->szStartTime.iSecond);
                sprintf(endtime, "%02d%02d%02d", cond->szEndTime.iHour, cond->szEndTime.iMinute,
                        cond->szEndTime.iSecond);
                if (strcmp(starttime, nowt) <= 0 && strcmp(nowt, endtime) <= 0) {
                    return 1;
                }
            }
        }
            break;
        case CM_WALLTIME_TIMETYPE_YEARDAY:
        {
            if (tm->tm_year >= cond->szStartTime.iYear && tm->tm_year <= cond->szEndTime.iYear &&
                    tm->tm_mon == cond->szStartTime.iMon && tm->tm_mday == cond->szStartTime.iDay) {

                sprintf(starttime, "%02d%02d%02d",
                        cond->szStartTime.iHour, cond->szStartTime.iMinute,
                        cond->szStartTime.iSecond);

                sprintf(endtime, "%02d%02d%02d",
                        cond->szEndTime.iHour, cond->szEndTime.iMinute, cond->szEndTime.iSecond);
                if (strcmp(starttime, nowtime) <= 0 && strcmp(nowtime, endtime) <= 0) {
                    return 1;
                }
            }
        }
            break;
        case CM_WALLTIME_TIMETYPE_ONCE:
        {
            sprintf(starttime, "%04d%02d%02d%02d%02d%02d", cond->szStartTime.iYear,
                    cond->szStartTime.iMon, cond->szStartTime.iDay,
                    cond->szStartTime.iHour, cond->szStartTime.iMinute,
                    cond->szStartTime.iSecond);

            sprintf(endtime, "%04d%02d%02d%02d%02d%02d", cond->szEndTime.iYear,
                    cond->szEndTime.iMon, cond->szEndTime.iDay,
                    cond->szEndTime.iHour, cond->szEndTime.iMinute, cond->szEndTime.iSecond);
            if (strcmp(starttime, nowtime) == 0 && strcmp(nowtime, endtime) == 0) {
                return 1;
            }
        }
            break;
        default:
            return -1;
    }
    return 0;
}

int cm_parse_data(unsigned char *data, char *string)
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

int cm_parse_data_string(char *string, char *save, int arrsize, int maxlen, const char sign)
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

int cm_parse_data_char(unsigned char *data, char *string, int maxlen, const char *sign)
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

int cm_parse_string_int(char *string, int *save, int maxlen, const char *sign)
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
int cm_parse_string_imagerects(char *string, CM_SDK_ImageSize *imageSize, const char *sign)
{
    if (!string || !imageSize) return -1;

    CM_SDK_ImageSize *inImage = imageSize;

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

int cm_clear_parenthesis(char *save, char *string)
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
int cm_clear_parenthesis_self(char *string)
{
    if (string == NULL) return -1;
    
    char  save[1024] = {0};
    cm_clear_parenthesis(save, string);
    return sprintf(string, "%s", save);
}

int cm_remove_last_break(char *args)
{
    if (args == NULL) return -1;

    int  len = strlen(args);
    if (args[len-1] == '\n') 
        args[len-1] = '\0';

    return 0;
}

int cm_clear_string_char(char *origin, char *save, char c)
{
    char *p = origin;
    char *s = save;
    while (p && *p != '\0') {
        if (*p != c) {
            *s++ = *p++;
        } else {
            p++;
        }
    }
    *s = '\0';
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

int cm_get_ip_dev(const char *dev, char *ipaddr)
{
    int sock_get_ip;  
  
    struct   sockaddr_in *sin;  
    struct   ifreq ifr_ip;     
  
    if ((sock_get_ip=socket(AF_INET, SOCK_STREAM, 0)) == -1)  
    {  
         return -2;
    }  
     
    memset(&ifr_ip, 0, sizeof(ifr_ip));     
    strncpy(ifr_ip.ifr_name, dev, sizeof(ifr_ip.ifr_name) - 1);     
   
    if( ioctl( sock_get_ip, SIOCGIFADDR, &ifr_ip) < 0 )     
    {     
         return -1;
    }       
    sin = (struct sockaddr_in *)&ifr_ip.ifr_addr;     
    strcpy(ipaddr,inet_ntoa(sin->sin_addr));         
      
    close( sock_get_ip );  
      
    return  0;
}

#define h_addr h_addr_list[0]
char *cm_get_ip(char *dn_or_ip, const char *eth)
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

int cm_get_mac(char * mac, int len_limit, char *dev)    //返回值是实际写入char * mac的字符个数（不包括'\0'）
{
    struct ifreq ifreq;
    int sock;

    if ((sock = socket (AF_INET, SOCK_STREAM, 0)) < 0)
    {
        return -1;
    }
    strcpy (ifreq.ifr_name, dev);    //Currently, only get eth0

    if (ioctl (sock, SIOCGIFHWADDR, &ifreq) < 0)
    {
        return -2;
    }

    close(sock);

    return snprintf (mac, len_limit, "%02X:%02X:%02X:%02X:%02X:%02X", (unsigned char) ifreq.ifr_hwaddr.sa_data[0], (unsigned char) ifreq.ifr_hwaddr.sa_data[1], (unsigned char) ifreq.ifr_hwaddr.sa_data[2], (unsigned char) ifreq.ifr_hwaddr.sa_data[3], (unsigned char) ifreq.ifr_hwaddr.sa_data[4], (unsigned char) ifreq.ifr_hwaddr.sa_data[5]);
}

int cm_get_flow(const char *interface, unsigned long long *recv,unsigned long long *send, unsigned long long *total)
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

int cm_check_mac_valid(const char *mac_addr)
{
    int i=0;
    if(strlen(mac_addr)!=12)
    {
        return -1;
    }
    for(i=0;i<12;i++)
    {
        if(!isxdigit(mac_addr[i]))
        {
            return -1;
        }
    }
    return 1;
}

int cm_generate_mac_with_char(const char *mac, char *save, char c)
{
    if (!mac || !save) return -1;
    int i = 0;
    int j = 0;
    int len = strlen(mac);
    for (i = 1; i < len+1; i++) {
        save[j++] = mac[i-1];
        if (i%2 == 0 && i < len) {
            save[j++] = c;
        }
    }
    return 0;
}

///////////////
// Take String to Path and Name
// ex: /Path/Filename
// Warn: you must sure the path and name has enough space.
int cm_seperate_filename(char *orig, char *path, char *name)
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
const char *cm_itoa(int value, char *save)
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
int cm_atoi(const char *value, int *out)
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
int cm_codec_parse_pps(char *pps, int lenpps)
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
            //char *now = pn;  // remeber the address.
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
int cm_codec_parse(char *data, int len, char *pps, int *lenpps, char *sps, int *lensps)
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
    *lenpps = cm_codec_parse_pps(pps, *lenpps);
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
        char cmdname[288] = {0};
        if (d->d_type != DT_DIR) continue;
        snprintf(cmdname, sizeof(cmdname), "/proc/%s/cmdline", d->d_name);
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

int cm_read_file_data(const char *filename, char **data, int *len) {
    if (!filename || !data) return -1;
    FILE *f = fopen(filename, "r");
    if (!f) return -2;
    struct stat st;
    stat(filename, &st);
    if (len) *len = st.st_size;

    *data = (char*)malloc(*len);
    int ret = fread(*data, 1, *len, f);
    fclose(f);
    return ret;
}

const char *cm_time_string(time_t tm)
{
    static char tStr[16] = {0};
    time_t t = tm;
    if (tm <= 0) {
        t = time(NULL);
    }
    sprintf(tStr, "%ld", t);
    return tStr;
}

unsigned int cm_gettime_only_micro()
{
    struct timeval tv;
    int ret = gettimeofday(&tv, NULL);
    if (ret != 0) {
        return 0;
    }
    return tv.tv_usec;
}

unsigned long long cm_gettime_micro()
{
    struct timeval tv;
    int ret = gettimeofday(&tv, NULL);
    if (ret != 0) {
        return 0;
    }
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

unsigned long long cm_gettime_milli()
{
    struct timeval tv;
    int ret = gettimeofday(&tv, NULL);
    if (ret != 0) {
        return 0;
    }
    return ((unsigned long long )tv.tv_sec) * 1000 + (unsigned long long) tv.tv_usec / 1000;
}

int cm_format_time(char *save, int max, char *format)
{
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    return strftime(save, max, format, tm);
}

int cm_format_time_t(char *save, int max, char *format, time_t t)
{
    struct tm *tm = localtime(&t);
    return strftime(save, max, format, tm);
}

int cm_format_time_local(char *save, int max, char *format, struct tm *tm)
{
    return strftime(save, max, format, tm);
}

int cm_random_with_chars(char *result, int num, char *chars)
{
    int len = strlen(chars);
    srand((unsigned int)time(NULL));
    int i;
    for (i = 0; i < num; i++) {
        result[i] = chars[(rand() % len)];
    }
    return 0;
}

int cm_random_with_num_char(char *result, int num)
{
    char *num_char = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    return cm_random_with_chars(result, num, num_char);
}

int cm_random_with_num_char_sym(char *result, int num)
{
    char *num_char = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz,=+";
    return cm_random_with_chars(result, num, num_char);
}

int cm_string_compare(const char *src, const char *dst)
{
    if (!src || !dst) return -1;

    size_t slen = strlen(src);
    size_t dlen = strlen(dst);
    size_t clen = slen > dlen ? slen : dlen;
    return strncasecmp(src, dst, clen);
}

int _cm_isxdigit (int c)
{
  return (c >= '0' && c <= '9') || 
         (c >= 'a' && c <= 'f') ||
         (c >= 'A' && c <= 'F');
}

int _cm_isprint (int c)
{
  return 1;
}

uint32_t cm_hex2bin (void *bin, char hex[]) 
{
    uint32_t len, i;
    uint32_t x;
    uint8_t *p=(uint8_t*)bin;
    
    len = strlen (hex);
    
    if ((len & 1) != 0) {
        return 0; 
    }
    
    for (i=0; i<len; i++) {
        if (_cm_isxdigit((int)hex[i]) == 0) {
            return 0; 
        }
    }
    
    for (i=0; i<len / 2; i++) {
        sscanf (&hex[i * 2], "%2x", &x);
        p[i] = (uint8_t)x;
    } 
    return len / 2;
} 

void cm_bin2scr (void *bin, uint32_t len) 
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
        printf ("%c", (c=='\t' || !_cm_isprint (c)) ? '.' : c);
    }
  }
}

int cm_get_wifi_signal(const char *dev)
{
    int size = 10240, signal = 0;
    char buf[10240] = {0};
    char *ptr;

    char iwconfigstr[128] = {0};
    sprintf(iwconfigstr, "iwconfig %s | grep Signal > /tmp/wifi_signal", dev);
    system(iwconfigstr);

    FILE *f = fopen("/tmp/wifi_signal", "r");
    if (!f) return 0;
    fread(buf, 1, size, f);

    if (buf[0] != '\0') {
        ptr = strstr(buf, "Signal level=");
        if (ptr) {
            ptr += strlen("Signal level=");
            signal = atoi(ptr);
        }
    }
    fclose(f);

    return signal;
}

#include <sys/statvfs.h>

unsigned long long cm_get_ubi_size(const char *dev)
{
    struct statvfs st;
    int ret = statvfs(dev, &st);
    if (ret < 0) {
        return 0;
    }
    return (unsigned long long)st.f_bsize * st.f_blocks;
}

unsigned long long cm_get_ubi_available(const char *dev)
{
    struct statvfs st;
    int ret = statvfs(dev, &st);
    if (ret < 0) {
        return 0;
    }
    return st.f_bsize * st.f_bavail;
}

unsigned long long cm_get_ubi_free(const char *dev)
{
    struct statvfs st;
    int ret = statvfs(dev, &st);
    if (ret < 0) {
        return ret;
    }
    return st.f_bsize * st.f_ffree;
}

uint8_t cm_http_getfile(char *sUrl, char *sFileName)
{
    int ret = -1;
#if (USE_LIBCURL == 1)
    CURLcode res = CURLE_OK;
    CURL *pCurlEasyHandle = NULL;
    char cStationURL[1024];
    cStationURL[0] = '\0';
    FILE* file = NULL;


    if (sUrl == NULL || strlen(sUrl) == 0)
    {
        sFileName = NULL;
        return -1;
    }

    file = fopen( sFileName, "wb");
    if(file == NULL){
        printf("fopen error\n");
        return -1;
    }

    /** Add "&urlinbody=true" at the end of the URL **/
    sprintf(cStationURL, "%s", sUrl);
    pCurlEasyHandle = curl_easy_init();

    if (pCurlEasyHandle)
    {
        curl_easy_setopt(pCurlEasyHandle, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(pCurlEasyHandle, CURLOPT_DNS_CACHE_TIMEOUT, 0);
        curl_easy_setopt(pCurlEasyHandle, CURLOPT_URL, cStationURL);
        curl_easy_setopt(pCurlEasyHandle, CURLOPT_FOLLOWLOCATION, 1);

        curl_easy_setopt(pCurlEasyHandle, CURLOPT_WRITEDATA, file);


        res = curl_easy_perform(pCurlEasyHandle);

        curl_easy_cleanup(pCurlEasyHandle);

        fclose(file);
    }
    ret = res;
#endif

    return ret;
}

unsigned short cm_litte_to_big(unsigned short v)
{
    unsigned short ret = 0;
    ret |= (v & 0xff) << 8;
    ret |= (v >> 8 & 0xff);
    return ret;
}

unsigned short cm_big_to_little(unsigned short v)
{
    unsigned short ret = 0;
    ret |= (v & 0xff) << 8;
    ret |= (v >> 8 & 0xff);
    return ret;
}

// from last to take string from split
// clear @save yourself,
// you must sure save big enough
int cm_take_out_last_string(const char *str, char split, char *save)
{
    char *pos = rindex(str, split);
    if (pos) {
        strncpy(save, pos+1, str + strlen(str) - pos -1);
    }
    return 0;
}

unsigned long cm_get_file_size(const char *filename)
{
    unsigned long filesize = 0;
    FILE *fp = fopen(filename, "r");
    if (fp) {
        fseek(fp, 0L, SEEK_END);
        filesize = ftell(fp);
        fclose(fp);
    }
    return filesize;
}

int cm_wifi_signal_level(int signal)
{
    static int levels[4] = { -35, -50, -75, -90 };
    if (signal < levels[3]) {
        return 1;
    } else if (signal > levels[0]) {
        return 5;
    } else if (signal < levels[0] && signal > levels[1]) {
        return 4;
    } else if (signal < levels[1] && signal > levels[2]) {
        return 3;
    } else if (signal < levels[2] && signal > levels[3]) {
        return 2;
    }
    return 0;
}

/*=============== End of file: cm_utils.c ==========================*/
