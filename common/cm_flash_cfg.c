/**************************************************************
* cfg用来设置出厂配置，并对参数进行加密
* cfg set mac=845dd7001122
* cfg set ssid=airdisk
* cfg set encryption=none(wep,wpa,wpa2,mixed-wpa)
* cfg set password=00000000
* cfg set ip=192.168.222.254
* cfg set dhcp_start=xxx
* cfg set dhcp_end=xxx
* cfg set wpa_cipher=xxx(tkip,aes,tkip/aes)
* 同时，cfg还可以读取配置信息，
* 读取的格式为cfg get xxx
* 得到的参数会打印出来
* Author: liuxiaolong
* time: 2012-12-6
**************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define DRIVER_NAME 		"/dev/mtdblock1"  	//uboot mtd
#define START_INDEX	0x40000//256K//0x58000  //start 352K (space of mtdblock0 is 384K);
#define MAX_SPACE 0x42000//256K+4K//352K+4K

#define CFG_DATA_LEN		64
#define CFG_DATA_LEN_MAX	(CFG_DATA_LEN*3)

#define CFGFLAG "flag"
#define FW_VERSION "fw_version"
#define QQSN     "qqsn"
#define QQLICENCE "license"
#define QQPID "qqpid"

//update_flag:when update_flag=1,enter system2.else enter system1.

typedef enum {
	CFG_PARAM_MAC=0,   // 0
	CFG_PARAM_SSID,        // 1
	CFG_PARAM_ENCRYPTION,// 2
	CFG_PARAM_PASSWORD,// 3
	CFG_PARAM_IP,// 4
	CFG_PARAM_DHCP_START,   // 5
	CFG_PARAM_DHCP_END,   // 6
	CFG_PARAM_WPA_CIPHER,   // 7
	CFG_PARAM_AIRPLAY_NAME,   // 8
	CFG_PARAM_DLNA_NAME,   // 9
	CFG_PARAM_FW_VERSION,   // 10
	CFG_PARAM_HOST_NAME,   // 11
	CFG_PARAM_MODEL_NAME,   // 12
	CFG_PARAM_MODEL_NUMBER,   // 13
	CFG_PARAM_MODEL_DESCRIPTION,   // 14
	CFG_PARAM_MANUFACTURER,   // 15
	CFG_PARAM_URL_MANUFACTURER,   // 16
	CFG_PARAM_DMS_NAME,   // 17
	CFG_PARAM_DMS_ENABLE,   // 18
	CFG_PARAM_FLAG,   // 19
	CFG_PARAM_QQSN,   // 20
	CFG_PARAM_LICENSE,   // 21
	CFG_PARAM_LICENSE_RES,   // 22
	CFG_PARAM_LICENSE_RES2,   // 23
	CFG_PARAM_QQPID,   // 24
	CFG_PARAM_VERSION_FLAG,   // 25
	CFG_PARAM_UPDATE_FLAG,   // 26
	CFG_PARAM_MQTT_DOMAINNAME,   // 27
	CFG_PARAM_MQTT_PREFIX,   // 28
	CFG_PARAM_MQTT_DEVICETYPE,   // 29
	CFG_PARAM_MQTT_DEVICEID,   // 30
	CFG_PARAM_MQTT_PRODUCT_ID,   // 31
	CFG_PARAM_MQTT_DEVICENAME,   // 32
	CFG_PARAM_MQTT_USERNAME,   // 33
	CFG_PARAM_MQTT_PW,   // 34
	CFG_PARAM_MQTT_TOKEN_ID,   // 35
	CFG_PARAM_COUNT
}cfg_param_type;

int g_parameter_data_len[] = {
    [CFG_PARAM_MAC] = CFG_DATA_LEN,
	[CFG_PARAM_SSID] = CFG_DATA_LEN,
	[CFG_PARAM_ENCRYPTION] = CFG_DATA_LEN,
	[CFG_PARAM_PASSWORD] = CFG_DATA_LEN, 
	[CFG_PARAM_IP] = CFG_DATA_LEN,
	[CFG_PARAM_DHCP_START] = CFG_DATA_LEN, 
	[CFG_PARAM_DHCP_END] = CFG_DATA_LEN,
	[CFG_PARAM_WPA_CIPHER] = CFG_DATA_LEN,      
	[CFG_PARAM_AIRPLAY_NAME] = CFG_DATA_LEN,
	[CFG_PARAM_DLNA_NAME] = CFG_DATA_LEN,
	[CFG_PARAM_FW_VERSION] = CFG_DATA_LEN,
	[CFG_PARAM_HOST_NAME] = CFG_DATA_LEN,     
	[CFG_PARAM_MODEL_NAME] = CFG_DATA_LEN,
	[CFG_PARAM_MODEL_NUMBER] = CFG_DATA_LEN,    
	[CFG_PARAM_MODEL_DESCRIPTION] = CFG_DATA_LEN,
	[CFG_PARAM_MANUFACTURER] = CFG_DATA_LEN,   
	[CFG_PARAM_URL_MANUFACTURER] = CFG_DATA_LEN,
	[CFG_PARAM_DMS_NAME] = CFG_DATA_LEN,     
	[CFG_PARAM_DMS_ENABLE] = CFG_DATA_LEN,
	[CFG_PARAM_FLAG] = CFG_DATA_LEN,
	[CFG_PARAM_QQSN] = CFG_DATA_LEN,
	[CFG_PARAM_LICENSE] = CFG_DATA_LEN_MAX,//CFG_DATA_LEN*3
	[CFG_PARAM_LICENSE_RES] = CFG_DATA_LEN,
	[CFG_PARAM_LICENSE_RES2] = CFG_DATA_LEN,
	[CFG_PARAM_QQPID] = CFG_DATA_LEN,
	[CFG_PARAM_VERSION_FLAG] = CFG_DATA_LEN, 
	[CFG_PARAM_UPDATE_FLAG] = CFG_DATA_LEN,
	[CFG_PARAM_MQTT_DOMAINNAME] = CFG_DATA_LEN,
	[CFG_PARAM_MQTT_PREFIX] = CFG_DATA_LEN,
	[CFG_PARAM_MQTT_DEVICETYPE] = CFG_DATA_LEN,
	[CFG_PARAM_MQTT_DEVICEID] = CFG_DATA_LEN,
	[CFG_PARAM_MQTT_PRODUCT_ID] = CFG_DATA_LEN,
	[CFG_PARAM_MQTT_DEVICENAME] = CFG_DATA_LEN,
	[CFG_PARAM_MQTT_USERNAME] = CFG_DATA_LEN,
	[CFG_PARAM_MQTT_PW] = CFG_DATA_LEN,
	[CFG_PARAM_MQTT_TOKEN_ID] = CFG_DATA_LEN,
	
	[CFG_PARAM_COUNT] = 0,

};

char *g_cfg_parameter_name[] = {
    [CFG_PARAM_MAC] = "mac",
	[CFG_PARAM_SSID] = "ssid",
	[CFG_PARAM_ENCRYPTION] = "encryption",
	[CFG_PARAM_PASSWORD] = "password", 
	[CFG_PARAM_IP] = "ip",
	[CFG_PARAM_DHCP_START] = "dhcp_start", 
	[CFG_PARAM_DHCP_END] = "dhcp_end",
	[CFG_PARAM_WPA_CIPHER] = "wpa_cipher",      
	[CFG_PARAM_AIRPLAY_NAME] = "airplay_name",
	[CFG_PARAM_DLNA_NAME] = "dlna_name",
	[CFG_PARAM_FW_VERSION] = "fw_version",
	[CFG_PARAM_HOST_NAME] = "host_name",     
	[CFG_PARAM_MODEL_NAME] = "model_name",
	[CFG_PARAM_MODEL_NUMBER] = "model_number",    
	[CFG_PARAM_MODEL_DESCRIPTION] = "model_description",
	[CFG_PARAM_MANUFACTURER] = "manufacturer",   
	[CFG_PARAM_URL_MANUFACTURER] = "url_manufacturer",
	[CFG_PARAM_DMS_NAME] = "dms_name",     
	[CFG_PARAM_DMS_ENABLE] = "dms_enable",
	[CFG_PARAM_FLAG] = "flag",
	[CFG_PARAM_QQSN] = "qqsn",
	[CFG_PARAM_LICENSE] = "license",
	[CFG_PARAM_LICENSE_RES] = "license_res",
	[CFG_PARAM_LICENSE_RES2] = "license_res2",
	[CFG_PARAM_QQPID] = "qqpid",
	[CFG_PARAM_VERSION_FLAG] = "version_flag", 
	[CFG_PARAM_UPDATE_FLAG] = "update_flag",
	[CFG_PARAM_MQTT_DOMAINNAME] = "mqtt_domainname",
	[CFG_PARAM_MQTT_PREFIX] = "mqtt_prefix",
	[CFG_PARAM_MQTT_DEVICETYPE] = "mqtt_devicetype",
	[CFG_PARAM_MQTT_DEVICEID] = "mqtt_deviceid",   
	[CFG_PARAM_MQTT_PRODUCT_ID] = "mqtt_product_id",
	[CFG_PARAM_MQTT_DEVICENAME] = "mqtt_deviceName",
	[CFG_PARAM_MQTT_USERNAME] = "mqtt_username",
	[CFG_PARAM_MQTT_PW] = "mqtt_pw",
	[CFG_PARAM_MQTT_TOKEN_ID] = "mqtt_token_id",
	
	[CFG_PARAM_COUNT] = NULL,
};

static void usage(void)
{
    int i = 0;
	printf("usage: cfg tool\n");
	printf("config parameters are: mac ssid encryption password ip dhcp_start dhcp_end wpa_cipher\n");
	printf("cfg set [param]=value: cfg set parameter=value \n");
	printf("cfg get [param]	     : cfg get parameter\n");
	printf("cfg list 	         : show all cfg parameter and value\n");
	printf("cfg checkflag 	     : check whether config parameters has been set\n");
	printf("cfg setflag 	     : set the flag to 1\n");
	printf("cfg clearflag 	     : set the flag to 0\n");
	printf("cfg erase 	         : erase all param to null\n");
	printf("param supported:>>>>>>>>\n");

	while(g_cfg_parameter_name[i])
	{
        printf("	%s  ",g_cfg_parameter_name[i]);
		printf("\n");
		i++;
	}
	printf("<<<<<<<<\n");

}

//检查mac地址是否合法
static int check_mac(char *mac_addr)
{
    int i=0;
    if(strlen(mac_addr)!=12)
    {
        return 1;
    }
    for(i=0;i<12;i++)
    {
        if(!isxdigit(mac_addr[i]))
        {
            return 1;
        }
    }
    return 0;
}

//获取参数
int cfg_get(FILE *fp,char *argv)
{
	unsigned char arg_str[CFG_DATA_LEN_MAX+1]={0};
	int i = 0,ret = -1,offset_index=0,j = 0;

//	if(argv==NULL)
//		return 1;

	while(g_cfg_parameter_name[i])
	{
		if(!strcmp(argv,g_cfg_parameter_name[i]))
		{
			offset_index = START_INDEX;
			for(j = 0; j< i;j++)
				offset_index+=g_parameter_data_len[j];
			if(offset_index >= MAX_SPACE)
			{
				printf("[WARNING] offset_index >= MAX_SPACE\n");
			}

			fseek(fp,offset_index,SEEK_SET);
			/*if(!strncmp(argv,QQLICENCE,strlen(QQLICENCE)))
          	  ret = fread(arg_str,CFG_DATA_LEN,3,fp);
			else*/
			ret = fread(arg_str,g_parameter_data_len[i],1,fp);
			if(ret < 0)
			{
				printf("[ERROR] read error!\n");
				return -1;
			}
			if(strlen((char*)arg_str) > 0)
				printf("%s=%s\n",argv,arg_str);
			break;
		}
		i++;
	}
	return 0;
}

int cm_flash_cfg_get(FILE *fp, char *argv, char *result)
{
	unsigned char arg_str[CFG_DATA_LEN_MAX+1]={0};
	int i = 0,ret = -1,offset_index=0,j = 0;

//	if(argv==NULL)
//		return 1;

	while(g_cfg_parameter_name[i])
	{
		if(!strcmp(argv,g_cfg_parameter_name[i]))
		{
			offset_index = START_INDEX;
			for(j = 0; j< i;j++)
				offset_index+=g_parameter_data_len[j];
			if(offset_index >= MAX_SPACE)
			{
				printf("[WARNING] offset_index >= MAX_SPACE\n");
			}

			fseek(fp,offset_index,SEEK_SET);
			/*if(!strncmp(argv,QQLICENCE,strlen(QQLICENCE)))
          	  ret = fread(arg_str,CFG_DATA_LEN,3,fp);
			else*/
			ret = fread(arg_str,g_parameter_data_len[i],1,fp);
			if(ret < 0)
			{
				printf("[ERROR] read error!\n");
				return -1;
			}
			if(strlen((char*)arg_str)) {
				sprintf(result, "%s", arg_str);
			}
			break;
		}
		i++;
	}
	return 0;
}

//设置参数
int cfg_set(FILE *fp,char *argv)
{
	char str_sp[CFG_DATA_LEN_MAX+1]={0};
	char value[CFG_DATA_LEN_MAX+1]={0};
	char temp_mac[32]={0};
	int offset_index = 0;
	int ret = 0;
	int i = 0,j = 0,k = 0;

//	if(argv==NULL)
//		return ret;
	while(g_cfg_parameter_name[i])
	{
		if(!strncmp(g_cfg_parameter_name[i],argv,strlen(g_cfg_parameter_name[i])))
		{
			memset(value,0x0,sizeof(value));
			strncpy(value,argv+strlen(g_cfg_parameter_name[i])+1,sizeof(value)-1);//ignore"xxx="
			if(i==0)//mac
			{
				if(check_mac(value))
				{
					return -1;
				}

				for(j = 0; j < 18; j+=3)
				{
					temp_mac[j] = *(value+(k++));
					temp_mac[j+1] = *(value+(k++));
					if(j<15)
						temp_mac[j+2] = ':';
				}
				sprintf(str_sp,"echo \'%s\' >/usr/data/mac.txt",temp_mac);
				system(str_sp);
				system("sync");
			}
			offset_index = START_INDEX;
			for(j = 0; j< i;j++)
				offset_index+=g_parameter_data_len[j];

			if(offset_index >= MAX_SPACE)
			{
				printf("[WARNING] offset_index >= MAX_SPACE\n");
			}
			fseek(fp,offset_index,SEEK_SET);
			/*if(!strncmp(argv,QQLICENCE,strlen(QQLICENCE)))
				ret = fwrite(value,CFG_DATA_LEN,3,fp);
			else*/
			ret = fwrite(value,g_parameter_data_len[i],1,fp);
			if(ret < 0)
			{
				printf("[ERROR] write error\n");
				return -1;
			}

			if(!strncmp(argv,QQSN,strlen(QQSN)))
			{
				system("mkdir -p /usr/data/");
				sprintf(str_sp,"echo \'%s\' >/usr/data/guid.txt",argv+strlen(QQSN)+1);
				system(str_sp);
			}
			else if(!strncmp(argv,QQLICENCE,strlen(QQLICENCE)))
			{
				system("mkdir -p /usr/data/");
				sprintf(str_sp,"echo \'%s\' >/usr/data/licence.txt",argv+strlen(QQLICENCE)+1);
				system(str_sp);
			}
			else if(!strncmp(argv,QQPID,strlen(QQPID)))
			{
				system("mkdir -p /usr/data/");
				sprintf(str_sp,"echo \'%s\' >/usr/data/qqpid.txt",argv+strlen(QQPID)+1);
				system(str_sp);
			}

			if(!strncmp(argv,CFGFLAG,strlen(CFGFLAG)) || !strncmp(argv,FW_VERSION,strlen(FW_VERSION)))
			{
				ret = 0;
			}
			else
			{
				ret = 1;
				system("sync");
			}
			break;
		}
		i++;
	}
	return ret;
}

//检查标志位，如果标志位为pisen，说明配置参数已经被设置，否则
//则没有被设置
static void check_cfg_flag(FILE *fp)
{
    cfg_get(fp,CFGFLAG);
}
//将标志位设置为pisen，如果参数被写入，则必须将标志位设为pisen
static void set_cfg_flag(FILE *fp)
{
	cfg_set(fp,"flag=1");
}
//清除标志位
static void clear_cfg_flag(FILE *fp)
{
    cfg_set(fp,"flag=0");
}

void cfg_erase(FILE *fp)
{
    int i = 0,offset_index = 0,j=0;
    char zeroarray[CFG_DATA_LEN]={0};
	while(g_cfg_parameter_name[i])
	{
		offset_index = START_INDEX;
		for(j=0; j < i; j++)
		{
            offset_index+=g_parameter_data_len[j];
		}
		if(offset_index >= MAX_SPACE)
		{
			printf("[WARNING] offset_index >= MAX_SPACE\n");
		}
		fseek(fp,offset_index,SEEK_SET);
		fwrite(zeroarray,g_parameter_data_len[i],1,fp);
		i++;
	}

}

void cfg_list(FILE *fp)
{
	int i=0;

	while(g_cfg_parameter_name[i])
	{
        cfg_get(fp,g_cfg_parameter_name[i]);
		//printf("\n");
		i++;
	}
}


int cm_flash_getmac(char *result)
{
	FILE *fp = fopen(DRIVER_NAME, "rb");
	if (fp) {
		cm_flash_cfg_get(fp, "mac", result);

		fclose(fp);
		return 0;
	}

	return -1;
}

int cm_flash_setmac(char *mac)
{
	FILE *fp = fopen(DRIVER_NAME, "rb+");
	if (fp) {
		char macargs[64] = {0};
		sprintf(macargs, "mac=%s", mac);
		int ret = cfg_set(fp, macargs);
		fclose(fp);
		return ret > 0 ? 0 : -2;
	}
	return -1;
}

int main1(int argc,char *argv[])
{
    FILE *fp = NULL;

	if(argc < 2 || !strcmp(argv[1],"-h"))
	{
		usage();
		exit(0);
	}

	if(!strcmp(argv[1],"get")&&argv[2])
	{
		fp=fopen(DRIVER_NAME,"rb");
		if(fp==NULL)
		{
			//printf("open /dev/mtdblock0 failed\n");
			exit(1);
		}
		cfg_get(fp,argv[2]);
	}
	else if(!strcmp(argv[1],"set")&&argv[2])
	{
		fp=fopen(DRIVER_NAME,"rb+");
		if(fp==NULL)
		{
			//printf("open /dev/mtdblock0 failed\n");
			exit(1);
		}
		if(cfg_set(fp,argv[2])>0 && strncmp(argv[2], "update_flag", strlen("update_flag")) != 0 )
	        set_cfg_flag(fp);
	}
	else if(!strcmp(argv[1],"checkflag"))
	{
		fp=fopen(DRIVER_NAME,"rb");
		if(fp==NULL)
		{
			//printf("open /dev/mtdblock0 failed\n");
			exit(1);
		}
		check_cfg_flag(fp);
	}
	else if(!strcmp(argv[1],"setflag"))
	{
		fp=fopen(DRIVER_NAME,"rb+");
		if(fp==NULL)
		{
			//printf("open /dev/mtdblock0 failed\n");
			exit(1);
		}
		set_cfg_flag(fp);
	}
	else if(!strcmp(argv[1],"clearflag"))
	{
		fp=fopen(DRIVER_NAME,"rb+");
		if(fp==NULL)
		{
			//printf("open /dev/mtdblock0 failed\n");
			exit(1);
		}
		clear_cfg_flag(fp);
	}
	else if(!strcmp(argv[1],"list"))
	{
		fp=fopen(DRIVER_NAME,"rb");
		if(fp==NULL)
		{
			//printf("open /dev/mtdblock0 failed\n");
			exit(1);
		}
		cfg_list(fp);
	}
	else if(!strcmp(argv[1],"erase"))
	{
		fp=fopen(DRIVER_NAME,"rb+");
		if(fp==NULL)
		{
			//printf("open /dev/mtdblock0 failed\n");
			exit(1);
		}
		cfg_erase(fp);
	}/**/
	else
	{
		usage();
	}
	if(fp)
		fclose(fp);
	return 0;
}
