/*================================================================
*   
*   File Name: wifi_json.c
*   Author: CHEN Z.P.
*   Mail: czp@sctek.cn
*   Created Time: 2018-06-26 16:58:00
*
================================================================*/

#include <json-c/json.h>
#include "linklist_interface.h"

#include "wifi_json.h"
//#include "common.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define WPA_CONF "/usr/data/wpa_supplicant.conf"
char *wpa_buf = "ctrl_interface=/var/run/wpa_supplicant/\nap_scan=1\n";

#define SC_DEBUG(fmt, ...) \
    do { \
    	printf(fmt, ##__VA_ARGS__); \
    }while(0);

int hasEncrypt=0;
int haswpa=0;
int encrypttype=0;

char macaddr[100];
char ssidstr[100];
char channelStr[10];
char rssi[10];
int rssidbm;

 bool find = false, discover = false;
 bool new_config = true;

 char* xmlEncode(char *string)  
{
	int destlen = 0;
	unsigned char *src, *dest;
	unsigned char *newstr;
	if (string == NULL) return NULL;

	for (src = (unsigned char *)string; *src != '\0'; src++)   	
	{
	   if (*src == '>')
	   	{
		   	destlen+=5;
		}else if(*src == '<')
			{
		   	destlen+=5;
		} else if(*src == '&')
			{
		   	destlen+=6;
		}
		  else if(*src == '\"')
			{
		   	destlen+=7;
		}
	   	else destlen++;
	}
	newstr = (unsigned char *)malloc(destlen + 1);
	memset(newstr,0,(destlen + 1));
	src = (unsigned char *)string;
	dest = newstr;

	while (*src != '\0')  
	{
		if (*src == '>')
		{
			*dest++ = '&';
			*dest++ = 'g';
			*dest++ = 't';
			*dest++ = ';';
			src ++;
		} 
		else if (*src == '<') 
		{
			*dest++ = '&';
			*dest++ = 'l';
			*dest++ = 't';
			*dest++ = ';';
			src++;
		} 
		else if (*src == '&') 
		{
			*dest++ = '&';
			*dest++ = 'a';
			*dest++ = 'm';
			*dest++ = 'p';
			*dest++ = ';';
			src++;
		} 
		else if (*src == '\"') 
		{
			*dest++ = '&';
			*dest++ = 'q';
			*dest++ = 'u';
			*dest++ = 'o';
			*dest++ = 't';
			*dest++ = ';';
			src++;
		} 
		else
		{
			*dest++ = *src++;
		}
	}
	
	*dest = 0;

   return (char*)newstr;
}

 char *xmldecode(char *xml)
 {
		 int i,j,k;
		 char *ret;
		 int f;
		 char tmp[10];
		 
		 i = strlen(xml);
		 ret = (char *)malloc(i);
		 
		 k=0;
		 for(j=0; j<i;)
		 {
				 if(xml[j] != '&')
				 {
					 ret[k]=xml[j];
					 k++;
					 j++;
				 }
				 else
				 {
					 j++;
					 f=0;
					 while(xml[j] != ';')
					 {
							 tmp[f++]=xml[j];
							 j++;
					 }
					 tmp[f]=0;
					 
					 if(strcmp(tmp, "lt") == 0)
					 {
							 ret[k]='<';
					 }
					 else if(strcmp(tmp, "gt") == 0)
					 {
							 ret[k]='>';
					 }
					 else if(strcmp(tmp, "amp") == 0)
					 {
							 ret[k]='&';
					 }
					 else if(strcmp(tmp, "apos") == 0)
					 {
							 ret[k]='\'';
					 }	 
					 else if(strcmp(tmp, "quot") == 0)
					 {
							 ret[k]='\"';
					 }		 
					 k++;
					 j++;												 
				 }
		 }		 
		 ret[k]=0;
		 return ret;
 }

void logstr(char *str)
{		
#if 0
	int fw_fp;
	int f_size;
	if( (fw_fp=fopen("/tmp/feng.txt","a+"))==NULL)    // write and read,binary
	{
		exit(1);
	}		
	
	f_size=fwrite(str,1,strlen(str),fw_fp);
	fputc('\n',fw_fp);

	fclose(fw_fp);
#endif
}

#define is_ascii  0x11
#define is_gb2312 0x22
#define is_utf8   0x33

int is_UTF8_or_gb2312(const char* str,long length)
{
   int i;
   int nBytes=0;//UFT8可用1-6个字节编码,ASCII用一个字节
   unsigned char chr;
   int bAllAscii=1; //如果全部都是ASCII, 说明不是UTF-8
   for(i=0;i<length;i++)
   {
      chr= *(str+i);
      if( (chr&0x80) != 0 ) // 判断是否ASCII编码,如果不是,说明有可能是UTF-8,ASCII用7位编码,但用一个字节存,最高位标记为0,o0xxxxxxx
        bAllAscii= 0;
      if(nBytes==0) //如果不是ASCII码,应该是多字节符,计算字节数
      {
         if(chr>=0x80)
         {
            if(chr>=0xFC&&chr<=0xFD)
             nBytes=6;
            else if(chr>=0xF8)
             nBytes=5;
            else if(chr>=0xF0)
             nBytes=4;
            else if(chr>=0xE0)
             nBytes=3;
            else if(chr>=0xC0)
             nBytes=2;
            else
            {
              return is_gb2312;
            }
            nBytes--;
         }
      }
      else //多字节符的非首字节,应为 10xxxxxx
      {
         if( (chr&0xC0) != 0x80 )
         {
            return is_gb2312;
         }
         nBytes--;
      }
   }

   if( nBytes > 0 ) //违返规则
   {
      return is_gb2312;
   }

   if( bAllAscii ) //如果全部都是ASCII, 说明不是UTF-8
   {
      return is_ascii;
   }
   return is_utf8;
}
/*
int code_convert(char *from_charset,char *to_charset,char *inbuf,int inlen,char *outbuf,int outlen)
{
	iconv_t cd;
	int rc;
	char **pin = &inbuf;
	char **pout = &outbuf;

	// cd = iconv_open(to_charset,from_charset);
	cd = iconv_open("utf-8", "gb2312");
	
	if (cd==0) return -1;
	memset(outbuf,0,outlen);
	if (iconv(cd,pin,&inlen,pout,&outlen)==-1) return -1;
	iconv_close(cd);
	return 0;
}

static int Gb2312ToUtf8(char *sOut, int iMaxOutLen, const char *sIn, int iInLen)     
{     
    char *pIn = (char *)sIn;     
    char *pOut = sOut;     
    size_t ret;     
    size_t iLeftLen=iMaxOutLen;     
    iconv_t cd;     
  
    cd = iconv_open("utf-8", "gb2312");     
    if (cd == (iconv_t) - 1)     
    {     
        return -1;     
    }     
    size_t iSrcLen=iInLen;     
    ret = iconv(cd, &pIn,&iSrcLen, &pOut,&iLeftLen);     
    if (ret == (size_t) - 1)     
    {     
        iconv_close(cd);     
        return -1;     
    }     
  
    iconv_close(cd);     
  
    return (iMaxOutLen - iLeftLen);     
}
*/

void formatScanStr(char *outstr)
{
	char tmp[30];
	char *ptmp;
	//int i;
	//int c_2_i_1=0;
	//int c_2_i_2=0;
	int type_ssid_code=0;
	if(!strlen(ssidstr))  //remove the empty ssid
		return;
	// logstr(ssidstr);
	type_ssid_code=is_UTF8_or_gb2312(ssidstr,strlen(ssidstr));
	if(type_ssid_code==is_gb2312)
	{
		// size_t inlen=strlen(ssidstr);
		// char *inbuf=(char *)malloc(inlen);
		// bzero( inbuf, inlen);
		// memcpy(inbuf,ssidstr,inlen);
		// size_t outlen = strlen(ssidstr) *4;
		// char *outbuf=(char *)malloc(outlen);
		// bzero( outbuf, outlen);

		// // code_convert(char *from_charset,char *to_charset,char *inbuf,int inlen,char *outbuf,int outlen);
		// code_convert("gb2312","utf-8",inbuf,inlen,outbuf,outlen);

		// Gb2312ToUtf8(outbuf, outlen, inbuf, inlen); 
		
		// logstr(outbuf);
		
		
		// memset(ssidstr,0,sizeof(ssidstr));

		// memcpy(ssidstr,outbuf,strlen(outbuf));
		// logstr("new ssid: ");
		// logstr(ssidstr);

		// free(inbuf);
		// free(outbuf);
		// // iconv_close(cd);
		return;
	}
	strcat(outstr, "{");


	
	strcat(outstr, "\"name\":\"");
	ptmp = xmlEncode(ssidstr);
	strcat(outstr, ptmp);
	free(ptmp);
	strcat(outstr, "\",");

	
	strcat(outstr, "\"mac\":\"");
	strcat(outstr, macaddr);
	strcat(outstr, "\",");

	strcat(outstr, "\"channel\":\"");
	strcat(outstr, channelStr);
	strcat(outstr, "\",");

	
	strcat(outstr, "\"rssi\":\"");
	sprintf(tmp, "%d", rssidbm);
	strcat(outstr, tmp);
	strcat(outstr, "\",");

	
	strcat(outstr, "\"encrypt\":\"");
	if(hasEncrypt)
	{
		if(haswpa)
		{
			if(haswpa&WPA1X_FLAG)
			{
				if((haswpa&WPA_FLAG) && (haswpa&WPA2_FLAG))
				{
					strcat(outstr, "WPA/WPA2-1X");
				}
				else 
				{
					if(haswpa&WPA_FLAG)
					{
						strcat(outstr, "WPA-1X");
					}
					else if(haswpa&WPA2_FLAG)
					{
						strcat(outstr, "WPA2-1X");
					}
				}
			}
			else
			{
				if((haswpa&WPA_FLAG) && (haswpa&WPA2_FLAG))
				{
					strcat(outstr, "WPA/WPA2-PSK");
				}
				else 
				{
					if(haswpa&WPA_FLAG)
					{
						strcat(outstr, "WPA-PSK");
					}
					
					if(haswpa&WPA2_FLAG)
					{
						strcat(outstr, "WPA2-PSK");
					}
				}
			}
		}
		else
		{
		//wep mode
			strcat(outstr, "WEP");
		}
	}
	else
	{
		//none
		strcat(outstr, "NONE");
	}
	strcat(outstr, "\",");


	
	strcat(outstr, "\"tkip_aes\":\"");
	if((encrypttype&ENCRYPT_CCMP) && (encrypttype&ENCRYPT_TKIP))
	{
		strcat(outstr, "tkip/aes");
	}
	else
	{
		if(encrypttype&ENCRYPT_CCMP)
		{
			strcat(outstr, "aes");
		}
		else if(encrypttype&&ENCRYPT_TKIP)
		{
			strcat(outstr, "tkip");
		}
	}
	
	strcat(outstr, "\"");

	strcat(outstr, "},");
}




/************************ ESSID SUBROUTINES ************************/
/*
 * The ESSID identify 802.11 networks, and is an array if 32 bytes.
 * Most people use it as an ASCII string, and are happy with it.
 * However, any byte is valid, including the NUL character. Characters
 * beyond the ASCII range are interpreted according to the locale and
 * the OS, which is somethign we don't control (network of other
 * people).
 * Routines in here try to deal with that in asafe way.
 */

/*------------------------------------------------------------------*/
/*
 * Escape non-ASCII characters from ESSID.
 * This allow us to display those weirds characters to the user.
 *
 * Source is 32 bytes max.
 * Destination buffer needs to be at least 129 bytes, will be NUL
 * terminated.
 */
void
iw_essid_escape(char *		dest,
		const char *	src,
		const int	slen)
{
  const unsigned char *	s = (const unsigned char *) src;
  const unsigned char *	e = s + slen;
  char *		d = dest;

  /* Look every character of the string */
  while(s < e)
    {

	#if 0
      int	isescape;

      /* Escape the escape to avoid ambiguity.
       * We do a fast path test for performance reason. Compiler will
       * optimise all that ;-) */
      if(*s == '\\')
	{
	  /* Check if we would confuse it with an escape sequence */
	  /*
	  if((e-s) > 4 && (s[1] == 'x')
	     && (isxdigit(s[2])) && (isxdigit(s[3])))
	    {
	      isescape = 1;
	    }
	  else*/
	    isescape = 0;
	}
      else
	isescape = 0;
      #endif

      /* Is it a non-ASCII character ??? */
	  /*
      if(isescape || !isascii(*s) || iscntrl(*s))
	{

	  sprintf(d, "\\x%02X", *s);
	  d += 4;
	}
      else*/
	{
	  /* Plain ASCII, just copy */
	  *d = *s;
	  d++;
	}
      s++;
    }

  /* NUL terminate destination */
  *d = '\0';
}


void changeMacStr(char *instr, char *outstr)
{
	//instr aa:bb:cc:dd:ee:ff
	//oustr aabbccddeeff
	outstr[0]=instr[0];
	outstr[1]=instr[1];
	outstr[2]=instr[3];
	outstr[3]=instr[4];
	outstr[4]=instr[6];
	outstr[5]=instr[7];
	outstr[6]=instr[9];
	outstr[7]=instr[10];
	outstr[8]=instr[12];
	outstr[9]=instr[13];
	outstr[10]=instr[15];
	outstr[11]=instr[16];
	outstr[13]=0;
}

int cgi_get_channel(char *ifname, char *chstr)
{
#ifndef NO_IWLIB
	int skfd;
	
	struct iwreq	  wrq;
	struct iw_range   range;
	double		  freq;
//	int 		  k;
	int 		  channel;

	if((skfd = iw_sockets_open()) < 0)
	{
		//error
		return -1;
	}

	
	if(iw_get_range_info(skfd, ifname, &range) < 0)
	{
		iw_sockets_close(skfd);
		return -1;//fprintf(stderr, "%-8.16s  no frequency information.\n\n",ifname);
	}
	else
	{
		/* Get current frequency / channel and display it */
		if(iw_get_ext(skfd, ifname, SIOCGIWFREQ, &wrq) >= 0)
		{
			freq = iw_freq2float(&(wrq.u.freq));
			channel = iw_freq_to_channel(freq, &range);
			if(channel > 0)
				sprintf(chstr, "%d", channel);
		}
	}

	iw_sockets_close(skfd);
#endif
	
	return 0;
	
}


#ifndef NO_IWLIB
/*------------------------------------------------------------------*/
/*
 * Output the link statistics, taking care of formating
 */
void
iw_get_dbm(char *		buffer,
	       int		buflen,
	       const iwqual *	qual,
	       const iwrange *	range,
	       int		has_range)
{
  int		len;


	/* Just do it...
	* The old way to detect dBm require both the range and a non-null
	* level (which confuse the test). The new way can deal with level of 0
	* because it does an explicit test on the flag. */
	if(has_range && ((qual->level != 0)
		   || (qual->updated & (IW_QUAL_DBM | IW_QUAL_RCPI))))
    {

		/* Check if the statistics are in RCPI (IEEE 802.11k) */
		if(qual->updated & IW_QUAL_RCPI)
		{
			/* Deal with signal level in RCPI */
			/* RCPI = int{(Power in dBm +110)*2} for 0dbm > Power > -110dBm */

		}
		else
		{
			/* Check if the statistics are in dBm */
			if((qual->updated & IW_QUAL_DBM)
			|| (qual->level > range->max_qual.level))
			{
				/* Deal with signal level in dBm  (absolute power measurement) */
				if(!(qual->updated & IW_QUAL_LEVEL_INVALID))
				{
					int	dblevel = qual->level;
					/* Implement a range for dBm [-192; 63] */
					if(qual->level >= 64)
						dblevel -= 0x100;
					//len = snprintf(buffer, buflen, "Signal level%c%d dBm  ",qual->updated & IW_QUAL_LEVEL_UPDATED ? '=' : ':',dblevel);
					//printf("%s\n", buffer);
					rssidbm = dblevel;
					buffer += len;
					buflen -= len;
				}

			}

		}
    }

}
#endif

/*------------------------------------------------------------------*/
/*
 * Parse, and display the results of a WPA or WPA2 IE.
 *
 */
static inline void 
iw_print_ie_wpa(unsigned char *	iebuf,
		int		buflen)
{
	int			ielen = iebuf[1] + 2;
	int			offset = 2;	/* Skip the IE id, and the length. */
	unsigned char		wpa1_oui[3] = {0x00, 0x50, 0xf2};
	unsigned char		wpa2_oui[3] = {0x00, 0x0f, 0xac};
	unsigned char *	wpa_oui;
	int			i;
	uint16_t		ver = 0;
	uint16_t		cnt = 0;

	if(ielen > buflen)
		ielen = buflen;


	switch(iebuf[0])
	{
		case 0x30:		/* WPA2 */
			/* Check if we have enough data */
			if(ielen < 4)
			{
				//iw_print_ie_unknown(iebuf, buflen);
				return;
			}

			wpa_oui = wpa2_oui;
		break;

		case 0xdd:		/* WPA or else */
			wpa_oui = wpa1_oui;

			/* Not all IEs that start with 0xdd are WPA. 
			* So check that the OUI is valid. Note : offset==2 */
			if((ielen < 8)
			|| (memcmp(&iebuf[offset], wpa_oui, 3) != 0)
			|| (iebuf[offset + 3] != 0x01))
			{
				//iw_print_ie_unknown(iebuf, buflen);
				return;
			}

			/* Skip the OUI type */
			offset += 4;
		break;

		default:
			return;
	}
  
	/* Pick version number (little endian) */
	ver = iebuf[offset] | (iebuf[offset + 1] << 8);ver++;ver--;
	offset += 2;

	if(iebuf[0] == 0xdd)
		haswpa |= WPA_FLAG;
	if(iebuf[0] == 0x30)
		haswpa |= WPA2_FLAG;

	/* From here, everything is technically optional. */

	/* Check if we are done */
	if(ielen < (offset + 4))
    {
		/* We have a short IE.  So we should assume TKIP/TKIP. */
		// printf("                        Group Cipher : TKIP\n");
		// printf("                        Pairwise Cipher : TKIP\n");
		//just TKIP
		encrypttype |= ENCRYPT_TKIP;
		return;
    }
 
	/* Next we have our group cipher. */
	if(memcmp(&iebuf[offset], wpa_oui, 3) != 0)
	{
		//printf("                        Group Cipher : Proprietary\n");
	}
	else
	{
		//printf("                        Group Cipher :");
		//iw_print_value_name(iebuf[offset+3], iw_ie_cypher_name, IW_IE_CYPHER_NUM);
		//printf("\n");
	}
	offset += 4;

	/* Check if we are done */
	if(ielen < (offset + 2))
	{
		/* We don't have a pairwise cipher, or auth method. Assume TKIP. */
		//printf("                        Pairwise Ciphers : TKIP\n");
		////just TKIP
		encrypttype |= ENCRYPT_TKIP;
		return;
	}

	/* Otherwise, we have some number of pairwise ciphers. */
	cnt = iebuf[offset] | (iebuf[offset + 1] << 8);
	offset += 2;
	//printf("                        Pairwise Ciphers (%d) :", cnt);

	if(ielen < (offset + 4*cnt))
		return;

	for(i = 0; i < cnt; i++)
	{
		if(memcmp(&iebuf[offset], wpa_oui, 3) != 0)
		{
			//printf(" Proprietary");
		}
		else
		{
			//encryptmode
			if(iebuf[offset+3] == 0)
			{
				//none
			}
			else if(iebuf[offset+3] == 1)
			{
				//wep-40
				encrypttype |= ENCRYPT_WEP_40;
			}
			else if(iebuf[offset+3] == 2)
			{
				//tkip
				encrypttype |= ENCRYPT_TKIP;
			}
			else if(iebuf[offset+3] == 3)
			{
				//wrap
				encrypttype |= ENCRYPT_WRAP;
			}
			else if(iebuf[offset+3] == 4)
			{
				//ccmp
				encrypttype |= ENCRYPT_CCMP;
			}
			else if(iebuf[offset+3] == 5)
			{
				//wep-104
				encrypttype |= ENCRYPT_WEP_104;
			}			
		}
		offset+=4;
	}
 
	/* Check if we are done */
	if(ielen < (offset + 2))
		return;

	/* Now, we have authentication suites. */
	cnt = iebuf[offset] | (iebuf[offset + 1] << 8);
	offset += 2;
	//printf("                        Authentication Suites (%d) :", cnt);

	if(ielen < (offset + 4*cnt))
		return;

	for(i = 0; i < cnt; i++)
	{
		if(memcmp(&iebuf[offset], wpa_oui, 3) != 0)
		{
			//printf(" Proprietary");
		}
		else
		{

			if(iebuf[offset+3] == 1)
			{
				//802.1x
				//wpa-1x or wpa2-1x
				haswpa |= WPA1X_FLAG;
			}
			else if(iebuf[offset+3] == 2)
			{
				//psk
				//wpa-psk or wpa2-psk
			}
			else
			{
				//none
			}
		}
		offset+=4;
	}
 
	/* Check if we are done */
	if(ielen < (offset + 1))
		return;

	/* Otherwise, we have capabilities bytes.
	* For now, we only care about preauth which is in bit position 1 of the
	* first byte.  (But, preauth with WPA version 1 isn't supposed to be 
	* allowed.) 8-) */
	if(iebuf[offset] & 0x01)
	{
		//printf("                       Preauthentication Supported\n");
	}
}
 
/*------------------------------------------------------------------*/
/*
 * Process a generic IE and display the info in human readable form
 * for some of the most interesting ones.
 * For now, we only decode the WPA IEs.
 */
static inline void
iw_print_gen_ie(unsigned char *	buffer,
		int		buflen)
{
  int offset = 0;

  /* Loop on each IE, each IE is minimum 2 bytes */
  while(offset <= (buflen - 2))
    {
      /* Check IE type */
      switch(buffer[offset])
		{
		case 0xdd:	/* WPA1 (and other) */
		case 0x30:	/* WPA2 */
		  iw_print_ie_wpa(buffer + offset, buflen);
		  break;
		default:
		  break;
		}
      /* Skip over this IE to the next one in the list. */
      offset += buffer[offset+1] + 2;
    }
}


/***************************** SCANNING *****************************/
/*
 * This one behave quite differently from the others
 *
 * Note that we don't use the scanning capability of iwlib (functions
 * iw_process_scan() and iw_scan()). The main reason is that
 * iw_process_scan() return only a subset of the scan data to the caller,
 * for example custom elements and bitrates are ommited. Here, we
 * do the complete job...
 */
#ifndef NO_IWLIB
/*------------------------------------------------------------------*/
/*
 * Print one element from the scanning results
 */
static inline void
print_scanning_token(struct stream_descr *	stream,	/* Stream of events */
		     struct iw_event *		event,	/* Extracted token */
		     struct iwscan_state *	state,
		     struct iw_range *	iw_range,	/* Range info */
		     int		has_range, char *str)
{
	char		buffer[128];	/* Temporary buffer */
//	char tmp[10];
	/* Now, let's decode the event */

	switch(event->cmd)
    {
    case SIOCGIWAP:
		//mac address
		if(state->ap_num != 1)			
		{
			formatScanStr(str);
			
			hasEncrypt = 0;
			haswpa = 0;
			encrypttype = 0;
		}
		
		iw_saether_ntop(&event->u.ap_addr, buffer);
		changeMacStr(buffer, macaddr);
		
		//printf("macaddr : %s\n", macaddr);
		state->ap_num++;
		break;
    case SIOCGIWNWID:
		break;
    case SIOCGIWFREQ:
		{
			double		freq;			/* Frequency/channel */
			int		channel = -1;		/* Converted to channel */
			freq = iw_freq2float(&(event->u.freq));
			channel = iw_freq_to_channel(freq, iw_range);
			sprintf(channelStr, "%d", channel);
		}
		break;
	case SIOCGIWMODE:
		break;
	case SIOCGIWNAME:
		break;
	case SIOCGIWESSID:
		{
			char essid[4*IW_ESSID_MAX_SIZE+1];
			memset(essid, '\0', sizeof(essid));
			if((event->u.essid.pointer) && (event->u.essid.length))
				iw_essid_escape(essid,event->u.essid.pointer, event->u.essid.length);
			
			if(event->u.essid.flags)
			{
				#if 0
				/* Does it have an ESSID index ? */
				if((event->u.essid.flags & IW_ENCODE_INDEX) > 1)
					printf("                    ESSID:\"%s\" [%d]\n", essid,(event->u.essid.flags & IW_ENCODE_INDEX));
				else
					printf("                    ESSID:\"%s\"\n", essid);
				#endif
				
				strcpy(ssidstr, essid);
			}
			else
			{
				ssidstr[0]=0;
			}
		}
		break;
    case SIOCGIWENCODE:
		{
			unsigned char	key[IW_ENCODING_TOKEN_MAX];
			if(event->u.data.pointer)
			  memcpy(key, event->u.data.pointer, event->u.data.length);
			else
			  event->u.data.flags |= IW_ENCODE_NOKEY;
			//printf("                    Encryption key:");
			if(event->u.data.flags & IW_ENCODE_DISABLED)
			{
				//encrypt = none;
			}
			else
			{
				hasEncrypt = 1;
			}
		}
		break;
    case SIOCGIWRATE:
		break;
    case SIOCGIWMODUL:
      break;
    case IWEVQUAL:
	//signal level
      iw_get_dbm(buffer, sizeof(buffer),&event->u.qual, iw_range, has_range);
      break;
    case IWEVGENIE:
      /* Informations Elements are complex, let's do only some of them */
      iw_print_gen_ie(event->u.data.pointer, event->u.data.length);
      break;
    case IWEVCUSTOM:
      break;
    default:
		break;
   }	/* switch(event->cmd) */

}
#endif
/*------------------------------------------------------------------*/
/*
 * Perform a scanning on one device
 */
static int cgi_get_scan(int skfd, char *	ifname,char *scanstr)		/* Args count */
{
#ifndef NO_IWLIB
	struct iwreq		wrq;
	struct iw_scan_req    scanopt;		/* Options for 'set' */
	int			scanflags = 0;		/* Flags for scan */
	unsigned char *	buffer = NULL;		/* Results */
	int			buflen = IW_SCAN_MAX_DATA; /* Min for compat WE<17 */
	struct iw_range	range;
	int			has_range;
	struct timeval	tv;				/* Select timeout */
	int			timeout = 15000000;		/* 15s */

	/* Get range stuff */
	has_range = (iw_get_range_info(skfd, ifname, &range) >= 0);

	/* Check if the interface could support scanning. */
	if((!has_range) || (range.we_version_compiled < 14))
    {
      //fprintf(stderr, "%-8.16s  Interface doesn't support scanning.\n\n",
	  //    ifname);
      return(-1);
    }

	/* Init timeout value -> 250ms between set and first get */
	tv.tv_sec = 0;
	tv.tv_usec = 250000;

	/* Clean up set args */
	memset(&scanopt, 0, sizeof(scanopt));


	/* Check if we have scan options */
	if(scanflags)
    {
      wrq.u.data.pointer = (caddr_t) &scanopt;
      wrq.u.data.length = sizeof(scanopt);
      wrq.u.data.flags = scanflags;
    }
	else
    {
      wrq.u.data.pointer = NULL;
      wrq.u.data.flags = 0;
      wrq.u.data.length = 0;
    }

	/* If only 'last' was specified on command line, don't trigger a scan */
	if(scanflags == IW_SCAN_HACK)
    {
      /* Skip waiting */
      tv.tv_usec = 0;
    }
	else
    {
		/* Initiate Scanning */
		if(iw_set_ext(skfd, ifname, SIOCSIWSCAN, &wrq) < 0)
		{
		  if((errno != EPERM) || (scanflags != 0))
		    {
		      //fprintf(stderr, "%-8.16s  Interface doesn't support scanning : %s\n\n",
			  //    ifname, strerror(errno));
		      return(-1);
		    }
		  /* If we don't have the permission to initiate the scan, we may
		   * still have permission to read left-over results.
		   * But, don't wait !!! */
		  tv.tv_usec = 0;
		}
    }
	timeout -= tv.tv_usec;

	/* Forever */
	while(1)
    {
		fd_set		rfds;		/* File descriptors for select */
		int		last_fd;	/* Last fd */
		int		ret;

		/* Guess what ? We must re-generate rfds each time */
		FD_ZERO(&rfds);
		last_fd = -1;

		/* In here, add the rtnetlink fd in the list */

		/* Wait until something happens */
		ret = select(last_fd + 1, &rfds, NULL, NULL, &tv);

		/* Check if there was an error */
		if(ret < 0)
		{
			if(errno == EAGAIN || errno == EINTR)
			continue;
			//fprintf(stderr, "Unhandled signal - exiting...\n");
			return(-1);
		}

		/* Check if there was a timeout */
		if(ret == 0)
		{
			unsigned char *	newbuf;

crealloc:
			/* (Re)allocate the buffer - realloc(NULL, len) == malloc(len) */
			newbuf = realloc(buffer, buflen);
			if(newbuf == NULL)
			{
				if(buffer)
				free(buffer);
				//fprintf(stderr, "%s: Allocation failed\n", __FUNCTION__);
				return(-1);
			}
			
			buffer = newbuf;

			/* Try to read the results */
			wrq.u.data.pointer = buffer;
			wrq.u.data.flags = 0;
			wrq.u.data.length = buflen;
			if(iw_get_ext(skfd, ifname, SIOCGIWSCAN, &wrq) < 0)
			{
				/* Check if buffer was too small (WE-17 only) */
				if((errno == E2BIG) && (range.we_version_compiled > 16)
				&& (buflen < 0xFFFF))
				{
					/* Some driver may return very large scan results, either
					* because there are many cells, or because they have many
					* large elements in cells (like IWEVCUSTOM). Most will
					* only need the regular sized buffer. We now use a dynamic
					* allocation of the buffer to satisfy everybody. Of course,
					* as we don't know in advance the size of the array, we try
					* various increasing sizes. Jean II */

					/* Check if the driver gave us any hints. */
					if(wrq.u.data.length > buflen)
						buflen = wrq.u.data.length;
					else
						buflen *= 2;

					/* wrq.u.data.length is 16 bits so max size is 65535 */
					if(buflen > 0xFFFF)
						buflen = 0xFFFF;

		  			/* Try again */
		  			goto crealloc;
				}

				/* Check if results not available yet */
				if(errno == EAGAIN)
				{
					/* Restart timer for only 100ms*/
					tv.tv_sec = 0;
					tv.tv_usec = 100000;
					timeout -= tv.tv_usec;
					if(timeout > 0)
						continue;	/* Try again later */
				}

				/* Bad error */
				free(buffer);
				//fprintf(stderr, "%-8.16s  Failed to read scan data : %s\n\n",
				//  ifname, strerror(errno));
				return(-2);
		    }
			else
				/* We have the results, go to process them */
				break;
		}

	  /* In here, check if event and event type
	   * if scan event, read results. All errors bad & no reset timeout */
	}

	//strcat(scanstr, "{");
	strcat(scanstr, "\"ApList\": [");
	
	if(wrq.u.data.length)
    {
		struct iw_event		iwe;
		struct stream_descr	stream;
		struct iwscan_state	state = { .ap_num = 1, .val_index = 0 };
		int			ret;
      
		//printf("%-8.16s  Scan completed :\n", ifname);
		iw_init_event_stream(&stream, (char *) buffer, wrq.u.data.length);
		
		hasEncrypt = 0;
		haswpa = 0;
		encrypttype = 0;

		do
		{
			/* Extract an event and print it */
			ret = iw_extract_event_stream(&stream, &iwe,
			range.we_version_compiled);
			if(ret > 0)
			{
				print_scanning_token(&stream, &iwe, &state,&range, has_range, scanstr);
				//printf("scanstr %s\n", scanstr);
			}
		}
		while(ret > 0);

		if(state.ap_num != 1)
		{
			formatScanStr(scanstr);
			scanstr[strlen(scanstr)-1] = '\0';
		}
		//printf("\n");

    }
	else
	    ;//printf("%-8.16s  No scan results\n\n", ifname);
	    
	strcat(scanstr, "]");
//	strcat(scanstr, "}");

	free(buffer);
#endif
	return(0);
}


int wifi_json_scan(char *ifname, char *outstr)
{
#ifndef NO_IWLIB
	int skfd;
	int ret;

	if((skfd = iw_sockets_open()) < 0)
	{
		//error
		return -1;
	}

	ret = cgi_get_scan(skfd, ifname, outstr);ret++;ret--;
	//printf("%s\n", outstr);
	
	iw_sockets_close(skfd);
#endif

	return 0;
}


#define WPAWPA2PSK	"WPA\\/WPA2-PSK"		//json_object_to_json_string WPA/WPA2-PSK 转换成 "WPA\/WPA2-PSK
#define TKIP_AES	"tkip\\/aes"			//json_object_to_json_string tkip/aes 转换成 ""tkip\/aes""

/*
description: 获取json对象
参数：
    root:json对象
    keypath: 相对于root的路径，每层路径用“.”隔开
    value:查找到的json对象指针存放地址
返回：bool类型
使用方法：
一个json对象内容如下(指针root对应这个对象)：{"header":{"cmd":{"cmd1":"arg1", "cmd2":"arg2"}, "ctr":"msg"}, "data":["data1", "data2"]}
如果要获取键值对{"cmd2":"arg2"}则使用JsonGetObj(root, "header.cmd.cmd2", &obj)
*/
json_bool JsonGetObj(json_object *root, const char *keypath, struct json_object **value)
{
    json_object *sub_obj = root;
    char *sub_key;
    char path[240];
    char *str_path = path;
 
    if (keypath == NULL || root == NULL || is_error(root))
    {
        *value = NULL;
        return FALSE;
    }
 
    memset(path, 0, sizeof(path));
    strcpy(path, keypath);
	//printf("Key Path: %s\n", keypath);
	
    while((sub_key = strsep(&str_path, ".")) != NULL)
    {   
    	//printf("sub_key: %s\n", sub_key);
        if (json_object_object_get_ex(sub_obj, sub_key, &sub_obj) == FALSE)
        {
            *value = NULL; 
            return FALSE;
        }
    }
 
    *value = sub_obj;
    return TRUE;
}


int wifi_json_join_set(WIFI_JSON_INFO *wifiinfo)
{
	#if 0

	char cur_mode[3]="\0";
	
	char encrypt_config[32]="\0";
	char tkip_aes_config[16]="\0";
	
	char str_sp[64]="\0";
	char str_sp_wifilist[64]="\0";
	char ssid_bak[64]={0};
	
	if(strlen(wifiinfo->encrypt))
	{
		if(!strcmp(wifiinfo->encrypt,"NONE"))
			strcpy(encrypt_config,"none");
		else if(!strcmp(wifiinfo->encrypt,"WEP"))
			strcpy(encrypt_config,"wep");
		else if(!strcmp(wifiinfo->encrypt,"WPA-PSK"))
			strcpy(encrypt_config,"psk");
		else if(!strcmp(wifiinfo->encrypt,"WPA2-PSK"))
			strcpy(encrypt_config,"psk2");
		else if(!strcmp(wifiinfo->encrypt,WPAWPA2PSK))
			strcpy(encrypt_config,"mixed-psk");
		else
			NULL;
	}
	
	if(strlen(wifiinfo->tkipaes))
	{
		if(!strcmp(wifiinfo->tkipaes,"tkip"))
			strcpy(tkip_aes_config,"tkip");
		else if(!strcmp(wifiinfo->tkipaes,"aes"))
			strcpy(tkip_aes_config,"ccmp");
		else if(!strcmp(wifiinfo->tkipaes,TKIP_AES))
			strcpy(tkip_aes_config,"tkip+ccmp");
		else
			NULL;
	}

	if(strlen(wifiinfo->name))
	{
	    strncpy(ssid_bak,wifiinfo->name,sizeof(ssid_bak));
		if (mozart_ini_setkey(NETCONFIGPATH, NETCLIENTSECTION, "ssid", wifiinfo->name)) {
			printf("Could not write ssid=%s below %s in %s.\n", wifiinfo->name, NETCLIENTSECTION, NETCONFIGPATH);
		}
	}
	if(!strcmp(encrypt_config,"none") || !strcmp(encrypt_config,"wep"))
	{
		sprintf(str_sp,"%s",encrypt_config);
		//sprintf(str_sp_wifilist,"wifisavelist.@wifi-iface[50].encryption=%s",encrypt_config);
	}
	else
	{
		sprintf(str_sp,"%s+%s",encrypt_config,tkip_aes_config);
		//sprintf(str_sp_wifilist,"wifisavelist.@wifi-iface[50].encryption=%s+%s",encrypt_config,tkip_aes_config);
	}
	if (mozart_ini_setkey(NETCONFIGPATH, NETCLIENTSECTION, "encryption", str_sp)) {
		printf("Could not write encryption=%s below %s in %s.\n", str_sp, NETCLIENTSECTION, NETCONFIGPATH);
	}
	memset(str_sp,0,64);
	memset(str_sp_wifilist,0,64);

	if(strlen(wifiinfo->password))
	{
		if((name=(char *)malloc(strlen(passwordStr)+1))!=NULL)
		{
			char *ntmp;
		
			memset(name,0,strlen(passwordStr)+1);
			ntmp = xmldecode(wifiinfo->password);
			strcpy(name,ntmp);
			free(ntmp);
		}
		if (mozart_ini_setkey(NETCONFIGPATH, NETCLIENTSECTION, "key", wifiinfo->password)) {
			printf("Could not write key=%s below %s in %s.\n", wifiinfo->password, NETCLIENTSECTION, NETCONFIGPATH);
		}
		memset(str_sp,0,64);
		memset(str_sp_wifilist,0,64);
	}


	if(strlen(wifiinfo->channel))
	{
		if (mozart_ini_setkey(NETCONFIGPATH, NETRADIOSECTION, "channel", wifiinfo->channel)) {
			printf("Could not write channel=%s below %s in %s.\n", wifiinfo->channel, NETRADIOSECTION, NETCONFIGPATH);
		}
	}
	

	system("touch /tmp/wifi_client_is_connecting");
//	system("touch /tmp/hhzzccc");
	//system("set_client.sh >/dev/null 2>&1");
	if(0 == create_wpa_config_file(ssid_bak,wifiinfo->password,NULL))
	{
		/*mozart_event wifi_connecting_event = {
			.event = {
				.misc = {
					.name = "wifi",
					.type = "connecting",
				},
			},
			.type = EVENT_MISC,
		};
		
		if (mozart_event_send(wifi_connecting_event))
			printf("Send wifi connecting event failure\n");*/
	    //system("startsta.sh>/dev/null 2>&1");
	    //mozart_play_key("wifi_linking");

		///while(1);
		
		struct wifi_client_register wifi_info;
		memset(&wifi_info, 0, sizeof(wifi_info));
	/*	wifi_info.pid = getpid();
		wifi_info.reset = 1;
		wifi_info.priority = 3;
		strcpy(wifi_info.name, "web_config");*/

		register_to_networkmanager(wifi_info, NULL);
		
	    wifi_ctl_msg_t new_mode;
	    memset(&new_mode, 0, sizeof(wifi_ctl_msg_t));
	    new_mode.force = true;
	    new_mode.cmd = SW_STA;
		//strcpy(new_mode.name, "mozart");
		 //SC_DEBUG("[Warning] %s: request_wifi_mode here!\n", __func__);
	    if (!request_wifi_mode(new_mode))
		    SC_DEBUG("[Warning] %s: request_wifi_mode fail!\n", __func__);
	}
		
	system("rm -f /tmp/wifi_client_is_connecting");

	return 0;
	#endif
	return 0;
}

int wifi_json_join_unpack(const char *jsonstr, char *retstr, WIFI_JSON_INFO *wifiinfo)
{	
	char *name=NULL;
	char *encrypt=NULL;
	char *password=NULL;
	char *tkip_aes=NULL;
	char *channel=NULL;
	char errInfo[128] = {'\0'};	
	json_object *root = NULL;
	json_object *ap_obj = NULL;
	json_object *tmpObject = NULL;
	
	if ((jsonstr == NULL) || (wifiinfo == NULL)) {
		printf("ERR: JSON string or wifiinfo was NULL!\n");
	    return -1;
	}

	memset(wifiinfo, '\0', sizeof(WIFI_JSON_INFO));
	
	root = json_tokener_parse(jsonstr);
	if(!root) {
	    printf("ERR: get root faild !\n");
	    return -2;
	}

	if(JsonGetObj(root, SETSYSSTR"."FN_SET_JoinWireless"."JoinWireless_SET_AP, &ap_obj) != TRUE) {
		strcat((char *)errInfo, PARAMETER_ERROR);
		goto exit_normal;
	}

	#if 0
	char *nodeInfo;
	nodeInfo = json_object_to_json_string(ap_obj);
	printf("ap_obj:%s\n", nodeInfo);
	#endif
	
	if (json_object_object_get_ex(ap_obj, JoinWireless_SET_AP_name, &tmpObject)) {
		const char *ntmp;
		name = (char *)json_object_to_json_string(tmpObject);
		ntmp = xmldecode(name);
		memcpy(wifiinfo->name, ntmp+1, strlen(ntmp)-2);
	} else {
		strcat(errInfo, PARAMETER_ERROR);
		goto exit_normal;
	}
		
	if (json_object_object_get_ex(ap_obj, JoinWireless_SET_AP_encrypt, &tmpObject)) {
		encrypt = (char *)json_object_to_json_string(tmpObject);
		memcpy(wifiinfo->encrypt, encrypt+1, strlen(encrypt)-2);
		
	} else {
		strcat(errInfo, PARAMETER_ERROR);
		goto exit_normal;
	}
		
	if(strcmp(wifiinfo->encrypt,"NONE"))
	{
		if (json_object_object_get_ex(ap_obj, JoinWireless_SET_AP_password, &tmpObject)) {
			password = (char *)json_object_to_json_string(tmpObject);
			memcpy(wifiinfo->password, password+1, strlen(password)-2);
		} 
	}
	
	if( !strcmp(wifiinfo->encrypt,"WPA-PSK") || !strcmp(wifiinfo->encrypt,"WPA2-PSK") || !strcmp(wifiinfo->encrypt,WPAWPA2PSK) )
	{
		if (json_object_object_get_ex(ap_obj, JoinWireless_SET_AP_tkip_aes, &tmpObject)) {
			tkip_aes = (char *)json_object_to_json_string(tmpObject);
			memcpy(wifiinfo->tkipaes, tkip_aes+1, strlen(tkip_aes)-2);
		} 
		
	}
	if( !strlen(wifiinfo->name) || strlen(wifiinfo->name)>32)
	{
		strcat(errInfo, ERROR_SSID);
		goto exit_normal;
	}

	if( !strcmp(wifiinfo->encrypt,"WPA-PSK") || !strcmp(wifiinfo->encrypt,"WPA2-PSK") || !strcmp(wifiinfo->encrypt,WPAWPA2PSK) )
	{
		if( !strlen(wifiinfo->password) || strlen(wifiinfo->password)<8 )
		{
			strcat(errInfo, ERROR_PASSWORD);
			goto exit_normal;
		}
	}

	if (json_object_object_get_ex(ap_obj, JoinWireless_SET_AP_channel, &tmpObject)) {
			channel = (char *)json_object_to_json_string(tmpObject);
			memcpy(wifiinfo->channel, channel+1, strlen(channel)-2);
	} 

	if(root) {
		json_object_put(root);
	}
	sprintf(retstr, "{\"%s\":{\"Return\": {\"status\": \"true\",\"delay\": \"20\"}}}\r\n", SETSYSSTR);
	
	return 0;
	
exit_normal:
	
		if(root) {
			json_object_put(root);
		}
		
		if(strlen(errInfo)) {
			memset(retstr, '\0', sizeof(retstr));
			sprintf(retstr, "{\"%s\":{\"Return\": {\"status\": \"false\",\"msg\": %s}}}\r\n", SETSYSSTR, errInfo);
		}
		
		return 1;

}




void mylogstrTofile(const char*format, ...)
{		
#if 0
    char strlog[8096] = {0};
	int fw_fp;
	int f_size;
    va_list ap;

	if( (fw_fp=fopen("/tmp/jianghu.txt","a+"))==NULL)    // write and read,binary
	{
		exit(1);
	}		
	
    va_start(ap, format);
    vsnprintf(strlog, sizeof(strlog), format, ap);
	f_size=fwrite(strlog,1,strlen(strlog),fw_fp);
	fputc('\n',fw_fp);
    va_end(ap);
	fclose(fw_fp);
#endif
}


int rewrite_wpaconfig_file(List *list)
{
	WiFi_Info  *get_element;
	int j,wpa_conf_fd;
	char tmp[256] = {};

	wpa_conf_fd = open(WPA_CONF, O_RDWR | O_TRUNC, 00744);
	if(wpa_conf_fd < 0){
		perror("open file fail");
		return -1;
	}
	write(wpa_conf_fd, wpa_buf, strlen(wpa_buf));

	//printf("WiFi WPA Info List len = %ld\n", list_get_length(list));
	for(j = 0; j < list_get_length(list); j++){
		get_element = list_get_element(list, j);
		if(get_element == NULL){
			if(j == 0){
				printf("[ERROE]WPA Configure Info List Head is NULL!!!!\n");
				return -1;
			}
			else
				continue;
		}
		write(wpa_conf_fd, "network={\nscan_ssid=1\n", strlen("network={\nscan_ssid=1\n"));

		memset(tmp, 0, 256);
		sprintf(tmp, "ssid=%s\n", get_element->ssid);
		printf("#====SSID====#: %s\n",tmp);
		write(wpa_conf_fd, tmp, strlen(tmp));

		if(strncmp(get_element->key_mgmt, "NONE", strlen("NONE"))){
			memset(tmp, 0, 256);
			sprintf(tmp, "psk=%s\n", get_element->psk);
			printf("#====PSK====#: %s\n",tmp);
			write(wpa_conf_fd, tmp, strlen(tmp));
			//write(wpa_conf_fd, "key_mgmt=WPA-PSK\n", strlen("key_mgmt=WPA-PSK\n"));
			printf("#====Key_Mgmt====#: WPA_PSK\n");
		}
		else{
			write(wpa_conf_fd, "key_mgmt=NONE\n", strlen("key_mgmt=NONE\n"));
			printf("#====Key_Mgmt====#: NONE\n");
		}

		memset(tmp, 0, 256);
		sprintf(tmp, "bssid=%s\n", get_element->bssid);
		printf("#====BSSID====#: %s\n",tmp);
		write(wpa_conf_fd, tmp, strlen(tmp));

		memset(tmp, 0, 256);
		sprintf(tmp, "priority=%s\n", get_element->priority);
		printf("#====Priority====#: %s\n",tmp);
		write(wpa_conf_fd, tmp, strlen(tmp));

		write(wpa_conf_fd, "}\n", strlen("}\n"));
	}
	close(wpa_conf_fd);
	list_destroy(list, NULL);
	return 0;
}


int update_wpaconfig_file(char *ssid, char* pwd, char* bssid)
{
	FILE *fp;
	List wifi_info_list;
	WiFi_Info *information = NULL;
	char *ps, *pe, *buffer;
	int file_size = 0;
	char buf[256] = {}, tmp[64] = {};
	int prior = 0,size;

	fp = fopen(WPA_CONF,"r+");
	if(!fp){
		perror("fopen");
		return -1;
	}
#if 1
	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);

	fseek(fp, 0, SEEK_SET);
	buffer =  (char *)malloc(file_size * sizeof(char));
	memset(buffer, 0, file_size * sizeof(char));
	fread(buffer, file_size, sizeof(char), fp);
	if((ps = strstr(buffer,"priority="))){
		while((pe = strstr(ps,"priority="))){
			ps = strstr(pe,"\n");
			strncpy(tmp, pe + 9, strlen(pe + 9) - strlen(ps));
			if(prior < atoi(tmp))
				prior = atoi(tmp);
		}
		//printf("Wpa_supplicant Max Priority is %d\n",prior);
		free(buffer);
	}
#endif
	memset(tmp, 0, 64);
	sprintf(tmp, "\"%s\"", ssid);
	list_init(&wifi_info_list);
	fseek(fp, 0, SEEK_SET);
	while(fgets(buf, sizeof(buf), fp)){
		if(buf[0] == '#')
			continue;

		if (strstr(buf, "ssid=\"")) {
			information = (WiFi_Info *)malloc(sizeof(WiFi_Info));
			memset(information, 0, sizeof(WiFi_Info));
			size = strlen(strstr(buf, "ssid=")) - strlen("ssid=") - strlen("\n");
			strncpy(information->ssid, strstr(buf, "ssid=") + strlen("ssid="), size);
			printf("====SSID====: [%s]\n", information->ssid);
			printf("====tmp====: [%s]\n", tmp);
			if(!strncmp(information->ssid, tmp, strlen(tmp))){
				find = true;
				new_config = false;
			}
		}
		if (strstr(buf, "psk=")) {
			size = strlen(strstr(buf, "psk=")) - strlen("psk=") - strlen("\n");
			strncpy(information->psk, strstr(buf, "psk=") + strlen("psk="), size);
			if(find == true){
				if(strlen(pwd) != 0){
					memset(tmp, 0, 64);
					sprintf(tmp, "\"%s\"", pwd);
					if(strncmp(information->psk, tmp, strlen(tmp))){
						memset(information->psk, 0, strlen(information->psk));
						strncpy(information->psk, tmp, strlen(tmp));
					}
				}
				else{
					memset(information->psk, 0, strlen(information->psk));
					memset(information->key_mgmt, 0, strlen(information->key_mgmt));
					strncpy(information->key_mgmt, "NONE", strlen("NONE"));
				}
			}
			//printf("====PSK====: [%s]\n", information->psk);
		}
		if (strstr(buf, "key_mgmt=")) {
			size = strlen(strstr(buf, "key_mgmt=")) - strlen("key_mgmt=") - strlen("\n");
			strncpy(information->key_mgmt, strstr(buf, "key_mgmt=") + strlen("key_mgmt="), size);
			printf("====Key_Mgmt====: [%s]\n", information->key_mgmt);
			if(find == true){
				if(strlen(pwd) != 0){
					memset(information->key_mgmt, 0, strlen(information->key_mgmt));
					memset(tmp, 0, 64);
					sprintf(tmp, "\"%s\"", pwd);
					memset(information->psk, 0, strlen(information->psk));
					strncpy(information->psk, tmp, strlen(tmp));
				}
			}
		}
		if (strstr(buf, "bssid=")) {
			size = strlen(strstr(buf, "bssid=")) - strlen("bssid=") - strlen("\n");
			strncpy(information->bssid, strstr(buf, "bssid=") + strlen("bssid="), size);
			if(find == true){
				if(bssid != NULL){
					if(strncmp(information->bssid, bssid, strlen(bssid)))
						strncpy(information->bssid, bssid, strlen(bssid));
				}
			}
			printf("====BSSID====: [%s]\n", information->bssid);
		}
		if (strstr(buf, "priority=")) {
			size = strlen(strstr(buf, "priority=")) - strlen("priority=") - strlen("\n");
			strncpy(information->priority, strstr(buf, "priority=") + strlen("priority="), size);
			if(find == true){
				memset(tmp, 0, 64);
				sprintf(tmp, "%d", prior + 1);
				strncpy(information->priority, tmp, strlen(tmp));
				find = false;
			}
			printf("====Priority====: [%s]\n", information->priority);

			list_insert(&wifi_info_list,information);
		}
	}
	printf("new_config:%d\r\n",new_config);
	if(new_config == true){
		information = (WiFi_Info *)malloc(sizeof(WiFi_Info));
		memset(information, 0, sizeof(WiFi_Info));
		memset(tmp, 0, 64);
		sprintf(tmp, "\"%s\"", ssid);
		strncpy(information->ssid, tmp, strlen(tmp));

		if(strlen(pwd) != 0){
			memset(tmp, 0, 64);
			sprintf(tmp, "\"%s\"", pwd);
			strncpy(information->psk, tmp, strlen(tmp));
			memset(information->key_mgmt, 0, strlen(information->key_mgmt));
			//strncpy(information->key_mgmt, "WPA-PSK\n", strlen("WPA-PSK\n"));
		}
		else{
			memset(information->psk, 0, strlen(information->psk));
			strncpy(information->key_mgmt, "NONE\n", strlen("NONE\n"));
		}

		if(bssid != NULL)
			strncpy(information->bssid, bssid, strlen(bssid));

		memset(tmp, 0, 64);
		sprintf(tmp, "%d", prior + 1);
		strncpy(information->priority, tmp, strlen(tmp));

		list_insert(&wifi_info_list,information);
	}
	fclose(fp);
	new_config = true;
	rewrite_wpaconfig_file(&wifi_info_list);

	return 0;
}



int filter_special_char(char *pwd)
{
	int i;
	char tmp[64] = {}, ascii[8] = {};
	//printf("Before filter, recv pwd data is %s\n", pwd);
	strncpy(tmp, pwd, strlen(pwd));

	for(i = 0; i < strlen(pwd); i++){
		memset(ascii, 0 , 8);
		sprintf(ascii, "%d\n", tmp[i]);
		//printf("ASCII: %s\n",ascii);
	}
	return strlen(pwd);
}




int create_wpa_config_file(char* ssid , char* password, char* bssid)
{
	int wpa_conf_fd, len;
	char priority[32] = {0};
	char ssid_name[64] = {0};
	char psk[128] = {0}, pwd[64] = {0};
	char bssidl[24] = {0};
	//unsigned char mac_addr[ETH_ALEN];

	if(strlen(ssid) == 0){
		SC_DEBUG("[Warnning]: ssid is null !!!!!!\n");
		return 0;
	}
    //SC_DEBUG("aaaaaaaaaa");
	len = filter_special_char(password);
	if(len == 0){
		SC_DEBUG("The length of password is 0 and Unencrypted networks  %d\n",strlen(password));
		memset(pwd, 0, 64);
	}
	else{
		SC_DEBUG("Valid length of the password:%d\n",len);
		strncpy(pwd, password, len);
	}

	if(strlen(pwd) != 0){
		if((strlen(pwd) < 8) || (strlen(pwd) > 63)){
			SC_DEBUG("[ERROR]: Invalid passphrase length %d (expected: 8..63)  \n",strlen(pwd));
			return -1;
		}
	}
#if 0
	if(hwaddr_aton(value, mac_addr)){
		return -1;
	}
#endif
	if(bssid != NULL){
		if((strlen(bssid) != 17) && (strlen(bssid) != 0)){
			SC_DEBUG("[ERROR]: Invalid bssid format!!(expected: xx:xx:xx:xx:xx:xx,length:17 or 0)\n");
			return -1;
		}
	}

	if(access(WPA_CONF,F_OK) != 0){
		SC_DEBUG("[%s] is not exist, Create!\n",WPA_CONF);
		wpa_conf_fd = open(WPA_CONF, O_CREAT | O_RDWR | O_APPEND, 00644);
		if(wpa_conf_fd < 0){
			perror("open file fail");
			return -1;
		}

		sprintf(ssid_name, "ssid=\"%s\"\n", ssid);
		if(strlen(pwd) != 0)
			sprintf(psk, "psk=\"%s\"\n", pwd);
		sprintf(priority, "priority=%d\n", 1);

		write(wpa_conf_fd, wpa_buf, strlen(wpa_buf));
		write(wpa_conf_fd, "network={\nscan_ssid=1\n", strlen("network={\nscan_ssid=1\n"));
		write(wpa_conf_fd, ssid_name, strlen(ssid_name));
		if(strlen(pwd) != 0){
			write(wpa_conf_fd, psk, strlen(psk));
			//write(wpa_conf_fd, "key_mgmt=WPA-PSK\n", strlen("key_mgmt=WPA-PSK\n"));
		}
		else
			write(wpa_conf_fd, "key_mgmt=NONE\n", strlen("key_mgmt=NONE\n"));

		if(bssid != NULL){
			sprintf(bssidl,"bssid=%s\n",bssid);
			write(wpa_conf_fd, bssidl, strlen(bssidl));
		}
		else
			write(wpa_conf_fd, "bssid=\n", 7);

		write(wpa_conf_fd, priority, strlen(priority));
		write(wpa_conf_fd, "}\n", 2);
		close(wpa_conf_fd);
		goto exit;
	}
	if(update_wpaconfig_file(ssid, pwd, bssid) != 0)
		return -1;
exit:
	SC_DEBUG("%s create successfully!\n",WPA_CONF);
	return 0;
}




