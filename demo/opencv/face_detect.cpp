//
// Created by jmdvirus on 2017/6/8.
//

#include "stdafx.h"
#include "CVFaceDetect.h"
#include "BaseOperation.h"

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

int face_detect_with_split(const char *filename)
{
	int len = 1024 * 1024 * 10;
	char *data = new char[len];

	FILE *file = NULL;
#ifdef _WIN32
	fopen_s(&file, filename, "rb");
#else
	file = fopen(filename, "r");
#endif
	if (file)
	{
		len = fread(data, 1, len, file);
		fclose(file);
	}

	CVFaceDetect fd;
	CvScalar colors(10, 10, 200);

	cv::Size fsize(768, 304);
	NaluUnit nu;
	int position = 0;
	char *fdata = NULL;
	char *p = data;
	int scan_len = 0;
	for (;;)
	{
#if 1
		position = ReadNaluFromBuf(&nu, data, len, position);
		if (position < 0) break;
		nu.data -= 4;
		nu.size += 4;
#else
		
		p = data + scan_len;
		char *start = p;
		char *end = data + len -1;
		int goon_find = -1;
		bool find_key = false;
		bool find_one = false;
		do {
			if ( (*p == 0x0 && *(p + 1) == 0x0 && *(p + 2) == 0x0 && *(p + 3) == 0x1))
			{
				if (*(p + 4) == 0x67)
				{
					start = p;
					goon_find = 1;
					p++;
				}
				else
				{
					// skip this one, go to find new one
				
					if (goon_find == 4)
					{
						goon_find = -1;
						end = (p - 1);
						break;
					}
					else if (goon_find++ > 0)
					{
						p++;
						continue;
					}

					if (find_one)
					{
						end = (p - 1);
						find_one = false;
						break;
					}
					// just think it is valid other frame
					start = p;
					find_one = true;
					p++;
				}
			} 
			else
			{
				p++;
			}
		} while (1);

		int reallen = end - start;
		if (reallen <= 0) continue;
		char *realdata = (char*)malloc(reallen);
		memcpy(realdata, start, reallen);
		scan_len += reallen;
		LOG("------------- frame len [%d]\n", reallen);
#endif

#if 0
		char *t_file = "D:\\hello.h264";
		FILE *file = NULL;
		fopen_s(&file, t_file, "wb+");
		fwrite(realdata, 1, reallen, file);
		fclose(file);
#endif

#if 0
		printf("-------------- len [%d] [%x, %x, %x, %x, %x, %x]\n", nu.size, nu.data[0], nu.data[1], nu.data[2], nu.data[3], nu.data[4], nu.data[5]);
		if (fdata) delete fdata;
		fdata = new char[nu.size];
		memcpy(fdata, nu.data, nu.size);
#endif

		cv::Mat fsrc; // (fsize, CV_LOAD_IMAGE_ANYDEPTH, realdata);
		std::vector<char> videodata;
		for (int i = 0; i < nu.size; i++)
		{
			videodata.push_back(nu.data[i]);
		}
	
		fsrc = cv::imdecode(videodata, CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_COLOR);
#if 0
		cv::VideoCapture inputVideo(t_file);
		if (!inputVideo.isOpened())
		{
			LOG("Error open [%s]\n", t_file);
			continue;
		}
		inputVideo >> fsrc;
#endif

		cv::UMat src;
		fsrc.copyTo(src);
		if (src.empty()) continue;

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
	if (data)
	{
		delete data;
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
#if 1
		std::vector<cv::Rect> ret = fd.detectFaces(src);
		for (int i = 0; i < ret.size(); i++) {
			cv::Rect dr = ret[i];
			CvRect cr(dr.x, dr.y, dr.width, dr.height);
			printf("Out index [%d] [%d, %d, %d, %d]\n", i, dr.x, dr.y, dr.width, dr.height);

			rectangle(showframe, cr, colors, 4);
		}
#endif
#ifndef __NO_HIGHGUI
		cv::imshow("video", showframe);

		char keycode = cv::waitKey(30);
		if (keycode == 27) {
			break;
		}
#endif
	}

	return 0;
}

#include "DrawSomething.h"
#include "map"

std::map<std::string, std::string> cfg_data;

int read_config(const char *file, std::map<std::string, std::string> &data)
{
	FILE *f = NULL;
#ifdef _WIN32
	fopen_s(&f, file, "rb");
#else
	f = fopen(file, "r");
#endif
	if (f)
	{
		char line[128] = { 0 };
		while (true)
		{
			if (feof(f)) break;
			char *p = fgets(line, 128, f);
			if (!p)
			{
				break;
			}
			if (line[0] == '\0')
			{
				continue;
			}
			char key[64] = { 0 };
			char value[128] = { 0 };
#ifdef _WIN32
			char *sl = line;
			char *tosave = key;
			while(true)
			{
				if (*sl == '\0') break;
				if (*sl == '\r' || *sl == '\n')
				{
					sl++;
					continue;
				}
				if (*sl == '=')
				{
					tosave = value;
					sl++;
				}
				*tosave++ = *sl++;

			}
			int ret = 2;
//			int ret = sscanf_s(line, "%[^=]=%s", key, value);
#else
			int ret = sscanf(line, "%[^=]=%s", key, value);
#endif
			if (ret == 2)
			{
				data.insert(std::make_pair(key, value));
			}
		}
	}

	return 0;
}


int main(int argc, char **args) {
    const char *img = args[1];

	LOG("config file : %s\n", img);

	read_config(img, cfg_data);

	const char *cmd = cfg_data["cmd"].c_str();
    CVFaceDetect fd;

	if (strcmp(cmd, "yuv") == 0)
	{
		cv::Size size(atoi(cfg_data["yuv_width"].c_str()), atoi(cfg_data["yuv_height"].c_str()));
		fd.face_detect_draw_video(cfg_data["yuv_file"].c_str(), size);
	}
	else if (strcmp(cmd, "camera") == 0)
	{
		face_detect_from_video();
	}
	else if (strcmp(cmd, "image") == 0)
	{
		fd.face_detect_draw_image(img);
	}
	else if (strcmp(cmd, "h264_file") == 0)
	{
		face_detect_from_file(cfg_data["h264_file"].c_str());
	}
	else if (strcmp(cmd, "h264_data") == 0)
	{
		face_detect_with_split(cfg_data["h264_file"].c_str());
	}
	else if (strcmp(cmd, "imgproc") == 0)
	{
		// test
		DrawSomething ds;
		//ds.make_one_draw();
		//	ds.make_img_reduce(cfg_data["reduce_img"].c_str());
		//	ds.make_add_roi(cfg_data["img1"].c_str(), cfg_data["img2"].c_str());
		//	ds.make_add_weighted(cfg_data["img1"].c_str(), cfg_data["img2"].c_str());
		//	ds.make_split_merge(cfg_data["img1"].c_str(), cfg_data["img2"].c_str(), cfg_data["img3"].c_str());
		//	ds.make_control_image(cfg_data["img1"].c_str());
		//	ds.make_dft(cfg_data["img1"].c_str());
	}

	getchar();
    return 0;
}
