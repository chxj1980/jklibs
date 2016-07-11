/*
 *===================================================================
 *
 *          Name: rt_server_tcp.c
 *        Create: 2015年11月14日
 *
 *   Discription: 
 *       Version: 1.0.0
 *
 *        Author: zhangtao@roamter.com
 *
 *===================================================================
*/
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#ifdef _MAXOS
#include <netinet/in.h>
#else
//#include <linux/in.h>
#endif

#include <arpa/inet.h>
#include <string.h>
#include <rt_print.h>
#include <unistd.h>
#include <errno.h>

int sockfd;//定义两个描述符
int acceptfd = 0;                    //sockfd:
                            //socket函数返回的描述符,bind,accept函数中作为参数
                        //acceptfd:
                            //accept函数返回的描述符,write和read函数中做为参数
struct sockaddr_in s_add,c_add;//定义服务器端和客户端的地址信息结构体
int recv_data_len ;//接收到的数据长度
//*************************************************************
//函数名称：int rt_server_tcp_startconnect(int port)
//函数功能：在服务器端建立tcp通信连接，并监听端口
//直到客户端发送请求
//参数：int port——服务器要监听的端口
//返回值：
//       0	成功建立链接
//	-1	建立socket失败
//	-2	bind失败
//	-3	listen失败
//**************************************************************/
int rt_server_tcp_startconnect(unsigned int port) 
{
    int flag=1,length=sizeof(int);
	rtinfo("Hello,welcome to my server %d !\r\n", port);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == sockfd)
    {
        rterror("socket fail ! \r\n");
        return -1;
    }
    rtinfo("socket ok %d !\r\n", sockfd);
    
    /* 填充服务器端口地址信息，以便下面使用此地址和端口监听 */
    bzero(&s_add,sizeof(struct sockaddr_in));
    s_add.sin_family=AF_INET;
    s_add.sin_addr.s_addr=htonl(INADDR_ANY); /* 这里地址使用全0，即所有 */
    s_add.sin_port=htons(port);
    /* 使用bind进行绑定端口 */
    if( setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, length) == -1)
    {
        perror("setsockopt");
        exit(1);
    }
    if(-1 == bind(sockfd,(struct sockaddr *)(&s_add), sizeof(struct sockaddr)))
    {
        rterror("bind fail %s!\r\n", strerror(errno));
        return -2;
    }
    rtinfo("bind ok !\r\n");
    /* 开始监听相应的端口 */
    if(-1 == listen(sockfd,5))
    {
        rterror("listen fail !\r\n");
        return -3;
    }
    rtinfo("listen ok\r\n");
    return 0;
}

//*************************************************************
//函数名称：int rt_server_tcp_startaccept()
//函数功能：调用时即进入阻塞状态，等待用户进行连接，
        //在没有客户端进行连接时，程序停止在此处，
        //当有客户端进行连接时，程序马上执行一次，
        //然后再次循环到此处继续等待。
//参数：	无
//返回值：
//	0	程序正常退出
//	-1	accept失败
//**************************************************************/
int rt_server_tcp_startaccept()
{
    if (sockfd <= 0) return -1;
    socklen_t sin_size = sizeof(struct sockaddr_in);
    // 此处accept的第二个参数用于获取客户端的端口和地址信息。
    acceptfd = accept(sockfd, (struct sockaddr *)(&c_add), &sin_size);
    if(-1 == acceptfd)
    {
        rterror("accept fail ! %s\r\n", strerror(errno));
        return -1;
    }
   // rterror("accept ok!\r\nServer start get connect from %#x : %#x\r\n",
    //		ntohl(c_add.sin_addr.s_addr),ntohs(c_add.sin_port));

    rtinfo("accept ok!\r\nServer start get connect from %s : %#x\r\n",
    		inet_ntoa(c_add.sin_addr), ntohs(c_add.sin_port));
    return 0;
}    

//*************************************************************
//函数名称：int rt_server_tcp_send()
//函数功能：向客户端发送数据
//参数：    char *data		要发送的数据
//         int	data_len	要发送的数据字节长度
//返回值：
//	-1	发送失败
//	0	发送成功	
//**************************************************************/
int rt_server_tcp_send(char *data, int data_len)
{
    /* 这里使用write向客户端发送信息，也可以尝试使用其他函数实现 */
    	if(-1 == write(acceptfd,data,data_len))
    	{
    		rterror("write fail!\r\n");
    		return -1;
    	}
    	rtinfo("write ok!\r\n");
		return 0;
}
//*************************************************************
//函数名称：int rt_server_tcp_read()
//函数功能：接收数据
//参数：	int sockfd          对于服务器是accept()函数返回的已连接套接字描述符，
//                          对于客户端是调用socket()函数返回的套接字描述符
//	char *read_buf      是指向一个用于接收信息的数据缓冲区
//	int read_buf_size   指明接收数据缓冲区的大小
//返回值：
//	-1		接收失败
//	0		接收成功	
//**************************************************************/
int rt_server_tcp_read(char *read_buf)
{
	if((recv_data_len = recv(acceptfd, read_buf,BUFSIZ,0)) < 0)
	{
		rterror("read error!\n");
		return -1;
	}
	return recv_data_len;
}
//*************************************************************
//函数名称：int rt_server_tcp_closesocket()  
//函数功能：关闭socket
//参数：	无
//返回值：
//	0		关闭成功
//**************************************************************/
int rt_server_tcp_closesocket()
{
	close(sockfd);
	return 0;
}
//*************************************************************
//函数名称：int rt_server_tcp_closeaccept()  
//函数功能：关闭accept
//参数：	无
//返回值：
//	0		关闭成功
//**************************************************************/
int rt_server_tcp_closeaccept()
{
	close(acceptfd);
	return 0;
}
/*=============== End of file: rt_server_tcp.c ==========================*/                                                                                                               
