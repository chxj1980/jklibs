/*
 *===========================================================================
 *
 *          Name: demo_tcp.c
 *        Create: 2015年11月14日 星期六 
 *
 *   Discription: 
 *
 *        Author: zhangtao
 *         Email: zhangtao@roamter.com
 *
 *===========================================================================
 */
#include<stdio.h>
#include<stdbool.h>
#include<stdint.h>
#include<pthread.h>
#include<string.h>
#include<unistd.h>
#include"rt_server_tcp.h"
#include"rasp/kfrasp.h"
#include"rt_print.h"
//#include"bvpu_utils.h"

char read_buf[BUFSIZ];//定义接收缓冲区，BUFSIZ为系统定义的常量
bool cycle_flag = 1;//定义循环标志位
unsigned int num = 0;//用来计数创建的线程数
pthread_t tcp_process[10];//最多创建10个线程，本数组用来存放每个线程的信息
//extern KFConnectItemInfo device_info;//该结构体用于在rt_files_return_clientmac_via_clientip
//KFConnectItemInfo item;                                            //函数当中返回客户端和本机mac相关信息
//char BUFFER_client_ip [64];
//char BUFFER_router_mac [64]; 
char *file_PATH = "/home/xiaobin_xu/xqb/xqb_workspace/kfconfig/common/dhcp.leases";//如果文件位置发生变动，请在此处修改//这种容易发生改动的变量最好定义为全局变量
char BUFFER_client_mac [64] ;
char BUFFER_router_mac [64] ;

                                    // define in bvpu_utils.h
static int kf_rasp_generate_data_register_response(KFRasp rasp, KFConnectItemInfo *item)
{
    char data[2048] = {0};
    int len = 2048;
    int ret;
    KFRaspBody body;
    if (!rasp) return -1;

    kf_rasp_generate_header(rasp, KF_RASP_CMD_CONTROL, KF_RASP_SUBCMD_REGISTER, "0002");
   // KFRaspBody body;
    memset(&body, 0, sizeof(KFRaspBody));
    // tell function what you need want to do.
    body.iData = KFRASP_PARSE_DATA_CONNECTITEMINFO_RESULT;
    // The structs depends on the @iData
    sprintf(body.cii.szName, "%s", item->szName);
    sprintf(body.cii.szAddr, "%s", item->szAddr);
    sprintf(body.cii.szDeviceMac, "%s", item->szDeviceMac);
    sprintf(body.cii.szRouterMac, "%s", item->szRouterMac);
	//rtdebug("%s11111111111111", body.cii.szRouterMac);
    // As response, you must set resultCode success or fail.
    sprintf(body.ResultCode, "%s", "success");
    kf_rasp_set_body(rasp, &body);
    ret = kf_rasp_generate_data(rasp, data, &len);
    if (ret < 0) {
        rterror("generate data failed. %d", ret);
        return -2;
    }


//    printf("data is :%s",data);
//    rtinfo("len: %d, %d", len, ret);
        rtinfo("string: %s", data);
	rt_server_tcp_send(data,len);
    return 0;
}

static int  rt_files_return_deviceinfo_via_clientip(char *input_client_ip)
        //该函数执行完毕之后device_info结构体中将返回对应的数据
{
    int fsc;
    FILE *fp = NULL;//定义文件类型指针
    if((fp = fopen(file_PATH,"r")) == NULL)//以只读方式打开文件
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

       if( fscanf(fp, "%s", time_sign)==0)//遇到空格时结束
	{
	 return -1;
	}

       if((fsc=fscanf(fp, "%s", client_mac))==0)
	{
	 return -2;
	}
       if((fsc=fscanf(fp, "%s", client_ip))==0) 
	{
	 return -3;
	}
        if((fsc=fscanf(fp, "%s", client_name))==0) 
	{
	 return -4;
	}
        if((fsc=fscanf(fp, "%s", router_mac))==0) 
	{	
	 return -5;
	}
        if(strcmp(client_ip, input_client_ip)== 0)
        {
            rtinfo("client_ip is %s\n",client_ip);
            rtinfo("client_mac is %s\n",client_mac);
            rtinfo("client_name is %s\n",client_name);
            rtinfo("router_mac is %s\n",router_mac);
                //BUFFER_client_ip = client_mac;
                //BUFFER_router_mac = router_mac;
                strncpy(BUFFER_client_mac, client_mac, sizeof(BUFFER_client_mac));
                strncpy(BUFFER_router_mac,  router_mac, sizeof(BUFFER_router_mac));
         // printf("%s==============", BUFFER_router_mac);
		  // return 0;             
        }
        return 0;

     }

    return -2;
}


//*******************************************************************
//函数名称：int rt_tcp_process()
//函数功能：多线程要处理的函数打包
//参数：    无
//返回值：
//          0		正常退出
//          其他		异常退出
//*******************************************************************/
void *rt_tcp_commen_process(void *args)
{
    //该函数读取客户端发送来的数据，并将数据返回到数据缓冲区中
 //   rt_server_tcp_read(read_buf);
    KFRasp rasp = NULL;//初始化定义解析json数据的结构体
    KFConnectItemInfo item; 

    rt_server_tcp_read(read_buf);
     int ret = kf_rasp_init(&rasp);//解析数据初始化函数
    //初始化错误
    if (ret < 0)
    {
        rterror("rasp init failed!, %d", ret);
        //return -1;
    }
    //解析JSON数据
    ret = kf_rasp_parse_data(rasp, read_buf);//read_buf为客户端发来的数据
    //解析错误
    if (ret < 0)
    {
        rterror("rasp parse data failed! %d", ret);
        //return -2;
    }
    KFRaspBody *body = kf_rasp_body(rasp);//提取数据体
    if(body)
    {
        //数据格式有错误
        if (body->iData == 0)
        {
            rtinfo("fail Data: %s",body->ResultData);
        }
        else if(body->iData == KFRASP_PARSE_DATA_DEVICEINFO)
        {
            rtinfo("DeviceInfo:Name: %s\n", body->di.szName);
            rtinfo("DeviceInfo:Addr: %s\n", body->di.szAddr);
            //从dhcp.leases文件中查找IP地址所对应的MAC地址
            //KFConnectItemInfo device_info;//该结构体用来保存客户端和路由器的相关信息
            if(0 == rt_files_return_deviceinfo_via_clientip(body->di.szAddr))
            {
               sprintf(item.szName, "%s", body->di.szName);
               sprintf(item.szAddr, "%s", body->di.szAddr);
	      // sprinft(item.szDeviceMac, "%s", BUFFER_client_mac);
	      // sprinft(item.szRouterMac, "%s", BUFFER_router_mac);
               strncpy(item.szDeviceMac, BUFFER_client_mac, sizeof(item.szDeviceMac));
               strncpy(item.szRouterMac, BUFFER_router_mac, sizeof(item.szRouterMac));
		//printf("%s///////////////", item.szRouterMac);

               kf_rasp_generate_data_register_response(rasp, &item);
	//	rt_server_tcp_send("hello !!!", 1024);
                      
            } 
            //KFConnectItemInfo.szDeviceMac = client_mac;
        }
        else if(body->iData == KFRASP_PARSE_DATA_DEVICEINFO_RESULT)
        {

        }
        else if (body->iData == KFRASP_PARSE_DATA_BINDLISTS_RESULT)
        {
            KFBindLists *bl = &body->bl;
            size_t i;
            for(i=0;i < bl->counts; i++)
            {
                KFConnectItemInfo *ii = bl->cii + i;
                rtinfo("BindLists %d: DeviceMac: %s", i, ii->szDeviceMac);
                rtinfo("RouterMac:%s", ii->szRouterMac);
            }
            kf_rasp_parse_data_free(rasp);
        }
    }
    
    else
    {
        rterror("Someting wrong with body!\n");   
    }
    
   // rt_server_tcp_send("hello ", 24);
    kf_rasp_deinit(&rasp);//解析数据结束，释放空间 
    
   // rt_server_tcp_send("hello client! welcome to my server!\n", 50);
    return NULL;
}


int main()
{
    //建立好TCP 连接，监听4567端口
    int ret = rt_server_tcp_startconnect(4567);
    if (ret < 0) {
        rterror(" error start %d", ret);
        return 0;
    }
    //该循环可以实现多个客户端的接入
    while(cycle_flag)
    {
        //该函数使服务器进入阻塞状态，一直等待客户端的接入，
        //=若无客户端则在再此等待，直到与客户端建立链接
        rt_server_tcp_startaccept();
        if(num >= 10)//建立超过是个线程则计数重新开始
        {
            num = 0;
        }
        pthread_create(&tcp_process[num++],NULL,rt_tcp_commen_process,NULL);//该函数为所有新创建线程都要执行的函数打包
        usleep(50000);
    }
    rt_server_tcp_closeaccept();
    rt_server_tcp_closesocket();
    return 0;
}

/*=============== End of file: demo_tcp.c ==========================*/
