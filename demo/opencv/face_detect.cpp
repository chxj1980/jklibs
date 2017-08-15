//
// Created by jmdvirus on 2017/6/8.
//

#include "stdafx.h"
#include "CVFaceDetect.h"


/**
* _NaluUnit
* 内部结构体。该结构体主要用于存储和传递Nal单元的类型、大小和数据
*/
typedef struct _NaluUnit
{
	int type;
	int size;
	unsigned char *data;
} NaluUnit;

static int ReadNaluFromBuf(NaluUnit *nalu, char* buf, int len, int nalhead_pos)
{
	nalu->size = 0;
	int		naltail_pos = nalhead_pos;
	while (nalhead_pos < len)
	{
		/* search for nal header */
		if (buf[nalhead_pos++] == 0x00 &&
			buf[nalhead_pos++] == 0x00)
		{
			if (buf[nalhead_pos++] == 0x01)
				goto gotnal_head;
			else {
				/* cuz we have done an i++ before,so we need to roll back now */
				nalhead_pos--;
				if (buf[nalhead_pos++] == 0x00 &&
					buf[nalhead_pos++] == 0x01)
					goto gotnal_head;
				else
					continue;
			}
		}
		else
			continue;

		/* search for nal tail which is also the head of next nal */
	gotnal_head:
		/* normal case:the whole nal is in this m_pFileBuf */
		naltail_pos = nalhead_pos;
		while (naltail_pos < len)
		{
			if (buf[naltail_pos++] == 0x00 &&
				buf[naltail_pos++] == 0x00)
			{
				if (buf[naltail_pos++] == 0x01)
				{
					nalu->size = (naltail_pos - 3) - nalhead_pos;
					break;
				}
				else {
					naltail_pos--;
					if (buf[naltail_pos++] == 0x00 &&
						buf[naltail_pos++] == 0x01)
					{
						nalu->size = (naltail_pos - 4) - nalhead_pos;
						break;
					}
				}
			}
		}

		nalu->type = buf[nalhead_pos] & 0x1f;
		nalu->data = (unsigned char*)buf + nalhead_pos;
		if (nalu->size == 0) {
			nalu->size = naltail_pos - nalhead_pos;
			return naltail_pos;
		}
		nalhead_pos = naltail_pos;
		return nalhead_pos - 4;
	}
	return -1;
}

int face_detect_from_video() {

	cv::VideoCapture vcap(0);

	CvScalar colors(10, 10, 200);
	cv::Size size(640, 480);
	cv::UMat frame;
	CVFaceDetect fd;
	while (1) {
		vcap >> frame;
		cv::Mat showframe;
		frame.copyTo(showframe);
#if 1
		std::vector<cv::Rect> ret = fd.detectFaces(frame);
		for (int i = 0; i < ret.size(); i++) {
			cv::Rect dr = ret[i];
			CvRect cr(dr.x, dr.y, dr.width, dr.height);
			printf("Out index [%d] [%d, %d, %d, %d]\n", i, dr.x, dr.y, dr.width, dr.height);
		
			rectangle(showframe, cr, colors, 4);
		}
#endif
#ifndef __NO_HIGHGUI
		cv::imshow("video", showframe);

		char keycode = cvWaitKey(30);
		if (keycode == 27) {
			break;
		}
#endif
	}
	return 0;
}

int face_detect_from_file(const char *filename)
{
	cv::VideoCapture inputVideo(filename);
	if (!inputVideo.isOpened())
	{
		return -1;
	}

	CVFaceDetect fd;
	CvScalar colors(10, 10, 200);
	cv::UMat src;
	for (;;)
	{
		inputVideo >> src;
		if (src.empty()) break;

		cv::Mat showframe;
		src.copyTo(showframe);
		std::vector<cv::Rect> ret = fd.detectFaces(src);
		for (int i = 0; i < ret.size(); i++) {
			cv::Rect dr = ret[i];
			CvRect cr(dr.x, dr.y, dr.width, dr.height);
			printf("Out index [%d] [%d, %d, %d, %d]\n", i, dr.x, dr.y, dr.width, dr.height);

			rectangle(showframe, cr, colors, 4);
		}
#ifndef __NO_HIGHGUI
		cv::imshow("video", showframe);

		char keycode = cvWaitKey(30);
		if (keycode == 27) {
			break;
		}
#endif
	}

	return 0;
}

int main(int argc, char **args) {
    const char *img = args[1];

//	face_detect_from_video();
    CVFaceDetect fd;
    //fd.face_detect_draw_image(img);

	face_detect_from_file(img);
//    cv::Size size(384, 288);
//    fd.face_detect_draw_video(img, size);
    return 0;
}
