#ifndef _PS_2_ES_H_
#define _PS_2_ES_H_

//本模块完成PS帧转ES帧的过程。
#define ES_FRM_AUDIO		0
#define ES_FRM_VIDEO		1

//StreamType的宏，目前仅仅支持如下类型
#define PS_STREAM_TYPE_H264			0x1B
#define PS_STREAM_TYPE_G711A		0x90

typedef void* PS2ES_HANDLE;

//创建模块。返回0，说明创建成功。
int PS2ES_Create(PS2ES_HANDLE *phPS2ES, int MaxESFrmLen);
//删除模块
void PS2ES_Delete(PS2ES_HANDLE hPS2ES);

//返回0，说明成功返回
//返回负值说明失败， 一般是内存没有了，因此不太可能发生
int PS2ES_PushIn(PS2ES_HANDLE hPS2ES, unsigned char *pPSFrm, int PSFrmLen);

//每次调用这个函数都应该一直调用，直到取不出为止。
//返回负值说明，内部缓冲已经取光了，不能再取了。
//返回0，说明成功，pTmStamp是时间戳，pPSFrm返回帧数据，FrmLen是长度。pFrmType返回帧类型
int PS2ES_PopOut(PS2ES_HANDLE hPS2ES, unsigned char *pESFrm, int *pESFrmLen, unsigned int *pTmStamp, int *pFrmType, unsigned char *pStreamType);


#endif
