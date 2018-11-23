/*
 *===========================================================================
 *
 *          Name: demo_tcp.c
 *        Create: 2015年11月14日 星期六 
 *
 *           Discription: 
 *
 *        Author: zhangtao
 *        Email: zhangtao@roamter.com
 *
 *===========================================================================
 */


#include <pthread.h>//创建多线程头文件
#include <string.h>
#include <stdio.h>//
#include <stdlib.h>
#include <unistd.h>//sleep函数头文件
#include "rt_server_tcp.h"//TCP通信头文件
#include "rt_print.h"//错误打印头文件



int cycle_flag = 1;//定义循环标志符
int num = 0;//创建线程计数
pthread_t tcp_pcocess[10];//声明线程对象数组，用于存储线程数据
//*************************************************************
//函数名称：int rt_tcp_process()
//函数功能：多线程要处理的函数打包
//参数：    无
//返回值：
//          0		正常退出
//          其他		异常退出
//**************************************************************/
void *rt_tcp_process(void *args)
{
    rtinfo("process creat success!\n");
    rt_server_tcp_send("hello client，I am server!\n", 30);//向客户端发送数据
    rtinfo("send massage to client success!\nwait 20s......\n");
    sleep(20);//休眠30s
    rtinfo("process end\n");
    return NULL;
}
int main()
{
    int ret = rt_server_tcp_startconnect(4567);//connect函数返回值
    if(ret != 0)
    {
        rterror("start tcp connect fail!\n",ret);
    }    
    while(cycle_flag)
    {
        if((ret = rt_server_tcp_startaccept()) != 0)
        {
            rterror("start tcp accept fail!\n",ret);
        }
        if(num < 10)
        {
            pthread_create(&tcp_pcocess[num++],NULL,rt_tcp_process,NULL);//i++  
            usleep(100000);//休眠100ms，释放CPU占用
        }
        if(num >= 10)
        {
            num = 0;
        }
    }
    rt_server_tcp_closesocket();
    rt_server_tcp_closeaccept();
    return 0;
}

/*=============== End of file: demo_tcp.c ==========================*/
