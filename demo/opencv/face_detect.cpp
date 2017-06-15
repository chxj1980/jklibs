//
// Created by jmdvirus on 2017/6/8.
//

#include "CVFaceDetect.h"

int face_detect_from_video() {

	cv::VideoCapture vcap(0);

	CvScalar colors(10, 10, 200);
	cv::Size size(640, 480);
	cv::UMat frame;
	CVFaceDetect fd;
	while (1) {
		vcap >> frame;
#if 1
		std::vector<cv::Rect> ret = fd.detectFaces(frame);
		for (int i = 0; i < ret.size(); i++) {
			cv::Rect dr = ret[i];
			CvRect cr(dr.x, dr.y, dr.width, dr.height);
			printf("Out index [%d] [%d, %d, %d, %d]\n", i, dr.x, dr.y, dr.width, dr.height);
//			cvRectangleR(frame, cr, colors, 4);
		}
#endif
		cv::imshow("video", frame);

		char keycode = cvWaitKey(30);
		if (keycode == 27) {
			break;
		}
	}
	return 0;
}

int main(int argc, char **args) {
    const char *img = args[1];

//	face_detect_from_video();
    CVFaceDetect fd;
    //fd.face_detect_draw_image(img);

    cv::Size size(1280, 720);
    fd.face_detect_draw_video(img, size);
    return 0;
}
