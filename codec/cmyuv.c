/*********************************************************
 *
 * Filename: codec/cmyuv.c
 *   Author: jmdvirus
 *   Create: 2019年01月23日 星期三 13时57分03秒
 *
 *********************************************************/

#include <stdio.h>

int cm_yuy2_yuv420p(const char *data, int width, int height, char *outdata)
{
	if (!data || !outdata) return -1;

	int len_one = width * height * 2;
	int len_two = width * height * 3 /2;
	//printf("len_one, len_two [%d, %d]\n", len_one, len_two);
	char *p = (char*)data;
	char *py = outdata;
	char *pu = outdata + width * height;
	char *pv = outdata + width * height + width * height/4;
	int i;
	int flag = 0;
	for (i = 0; i < len_one; i+=4) {
		char yp1 = *p;
		char yp2 = *(p+2);
		char yu = *(p+1);
		char yv = *(p+3);
		*py++ = yp1;
        *py++ = yp2;
		if (flag == 0) {
			flag = 1;
		    *pu++ = yu;
		    *pv++ = yv;
		} else flag = 0;
		p += 4;
		//printf("now [i=%d]\n", i);
		//printf("[%x, %x, %x,%x]\n", yp1, yp2, yu, yv);
	}

	return 0;
}

