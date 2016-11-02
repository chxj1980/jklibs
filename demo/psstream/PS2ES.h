#ifndef _PS_2_ES_H_
#define _PS_2_ES_H_

//��ģ�����PS֡תES֡�Ĺ��̡�
#define ES_FRM_AUDIO		0
#define ES_FRM_VIDEO		1

//StreamType�ĺ꣬Ŀǰ����֧����������
#define PS_STREAM_TYPE_H264			0x1B
#define PS_STREAM_TYPE_G711A		0x90

typedef void* PS2ES_HANDLE;

//����ģ�顣����0��˵�������ɹ���
int PS2ES_Create(PS2ES_HANDLE *phPS2ES, int MaxESFrmLen);
//ɾ��ģ��
void PS2ES_Delete(PS2ES_HANDLE hPS2ES);

//����0��˵���ɹ�����
//���ظ�ֵ˵��ʧ�ܣ� һ�����ڴ�û���ˣ���˲�̫���ܷ���
int PS2ES_PushIn(PS2ES_HANDLE hPS2ES, unsigned char *pPSFrm, int PSFrmLen);

//ÿ�ε������������Ӧ��һֱ���ã�ֱ��ȡ����Ϊֹ��
//���ظ�ֵ˵�����ڲ������Ѿ�ȡ���ˣ�������ȡ�ˡ�
//����0��˵���ɹ���pTmStamp��ʱ�����pPSFrm����֡���ݣ�FrmLen�ǳ��ȡ�pFrmType����֡����
int PS2ES_PopOut(PS2ES_HANDLE hPS2ES, unsigned char *pESFrm, int *pESFrmLen, unsigned int *pTmStamp, int *pFrmType, unsigned char *pStreamType);


#endif
