#include "libcodec.h"
#include "SAVCodec.h"
#include <android/log.h>
#undef LOG_TAG
#define  LOG_TAG    "CODEC"
#define  LOGI(...) __android_log_print (ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

extern GlobalValue g_value;

CodecModule::CodecModule() :
		m_codecMutex(true) {
	m_exit = false;
	g_value.codecThread = &m_codecMutex;
}

CodecModule::~CodecModule() {

}

void CodecModule::run() {
	LOGI("CodecModule::run() = %d", gettid());
	while (!m_exit) {
		AutoLock lock(g_value.codecThread);
		if (g_value.codecEvent == EVENT_DIALOG_CREATED) {
			CCall *pDialog = (CCall*) g_value.pCodecUserData;
			g_value.codecEvent = 0;
			g_value.pCodecUserData = NULL;
			SAVCodec_Context* ctx = pDialog->GetCtxVideoDec();
			LOGI(
					"%d %d %d %d", ctx->eCodecID, ctx->stVideoParam.iWidth, ctx->stVideoParam.iHeight, ctx->iExtraDataSize);
			SAV_Result result = SAVCodec_Open(ctx);
			LOGI("result=%d", result);
			pDialog->InitIOBuf(DIALOGDATA_LIST_VIDEORENDER,
					ctx->stVideoParam.iWidth * ctx->stVideoParam.iHeight * 10,
					32);
		}
		list<CCall*>* pDialogList = g_value.pModule->getDialogList();
		list<CCall*>::iterator iter = pDialogList->begin();
		while (iter != pDialogList->end()) {
			CCall* pDialog = *iter;
			IOBuf* pVideoDeCode = pDialog->getIOBuf(DIALOGDATA_LIST_VIDEODEC);
			if (pVideoDeCode) {
				IOBuf::IOUnit *pUnit = pVideoDeCode->consumeBegin();
				if (pUnit) {

					SAV_Frame avFrame;
					avFrame.iSize = sizeof(avFrame);
					SAV_Packet packet;
					{
						static int pts = 20000;
						packet.iSize = sizeof(packet);
						packet.iPTS = pts;
						pts += 20000;
						packet.pData = (SAV_TYPE_UINT8*) pUnit->pBuf;
						packet.iDataSize = pUnit->data.packet.iDataSize;
					}
					int ret = SAVCodec_Process(pDialog->GetCtxVideoDec(),
							&avFrame, &packet);
					if (ret > 0 && avFrame.ePictType != SAV_PICTURE_TYPE_NONE) {
						int size = SAVImg_GetSize(
								&pDialog->GetCtxVideoDec()->stVideoParam, NULL);
						IOBuf* pVideoRender = pDialog->getIOBuf(
								DIALOGDATA_LIST_VIDEORENDER);
						if (pVideoRender) {
							IOBuf::IOUnit *pUnit2 = pVideoRender->produceBegin(
									size);
							if (pUnit2) {
								SAV_Frame dstFrame;
								memset(&dstFrame, 0, sizeof(dstFrame));
								int ret =
										SAVImg_Copy(
												&pDialog->GetCtxVideoDec()->stVideoParam,
												&avFrame, &dstFrame,
												pUnit2->pBuf);
								pUnit2->data.frame.iDataSize[0] = size;
								pUnit2->data.frame.ppData[0] = pUnit2->pBuf;
								pVideoRender->produceEnd(pUnit2);
							}
						}
					}
					pVideoDeCode->consumeEnd(pUnit);
				}
			}

			++iter;
		}
		//usleep(1);
	}
}

void CodecModule::cleanup() {

}

void CodecModule::exit() {
	m_exit = true;
}

int CodecModule::decodeVideo(SAVCodec_Context * pCtx, char* buf, int len) {

	return 0;
}
