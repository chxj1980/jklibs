#include <stdio.h>
#include <string.h>

#include "cm_uart.h"

unsigned short checksum(char* buffer, int size)
{
  		unsigned short crc = 0x0000;
		int	i = 0;

  		for(i = 0; i < size; i++)
                {
    			crc += (((unsigned short)buffer[i]) & 0x00FF);
			crc = ~((crc << 1) & 0xFFFE);
			crc += 1;
    			if((i % 8) == 0)
                        {
      			crc ^= 0x1021;
			}
 		 }
 	 	crc = (~crc) + 1;
 	 	return crc;
}

const char *req_data = "{\"api\":\"x1000/request/device_id\"}";
const char *req_wifi = "{\"api\":\"x1000/request/wifi_config\"}";

int main(int argc, char **args) {
	int fd = cm_uart_open(args[1]);
	printf("open [%s] ret [%d]\n", args[1], fd);
	int ret = cm_uart_init(fd, 19200, 0, 8, 1, 'n');
	printf("init ret [%d]\n", ret);

	char head[4] = "XTOC";
	int datalen = strlen(req_data);

	unsigned short alllen = datalen + 6;
	//unsigned short alllenx = htonl(alllen);
	unsigned short alllenx = 0;
	alllenx |= (alllen >> 8 & 0xff);
	alllenx |= (alllen & 0xff) << 8;

	char senddata[1024] = {0};
	memcpy(senddata, head, 4);
	memcpy(senddata+4, &alllenx, 2);
	memcpy(senddata+6, req_data, datalen);

	unsigned short cs = checksum((char*)senddata+6, datalen);
	unsigned short csx = 0;
	csx |= (cs >> 8) & 0xff;
	csx |= (cs & 0xff) << 8;
	memcpy(senddata+6+datalen, &csx, 2);

	ret = cm_uart_send(fd, senddata, datalen + 8);
	printf("------ send data [%d]\n", ret);

	int z;
	for (z = 0; z < alllen+2; z++) {
		printf("%02x ", senddata[z]);
	}
	printf("\n");
	char data[1024] = {0};
	int len = 1024;
	while (1) {
            int ret = cm_uart_recv(fd, data, len, 1, 0);
	    printf("get data ret [%d] len [%d] data [%s]\n", ret, len, data+6);
	    usleep(50000);
	}

	return 0;
}
