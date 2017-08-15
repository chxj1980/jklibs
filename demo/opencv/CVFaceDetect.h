//
// Created by jmdvirus on 2017/6/8.
//

#ifndef JKLIBS_CVFACEDETECT_H
#define JKLIBS_CVFACEDETECT_H

#include <vector>

using namespace std;

#define DEBUG_DURATION

class CVFaceDetect {
public:
    // Parse YUV frame
    // size: YUV size of video
    std::vector<cv::Rect> detect_face(const char *buffer, int len, cv::Size size);

	std::vector<cv::Rect> detect_face_h264(cv::UMat frame);

    // Parse one image
    std::vector<cv::Rect> detect_face_image(const char *filename);

    // Debug for view
    int face_detect_draw_image(const char *filename);
    int face_detect_draw_video(const char *filename, cv::Size size);

	int face_detect_draw_video_h264(const char *filename, cv::Size size);

    std::vector<cv::Rect>    face_rects() {
        return detect_face_rects_;
    }

	std::vector<cv::Rect> detectFaces(cv::UMat img_gray);
	std::vector<cv::Rect> detectFaces2(cv::Mat img_gray);

private:
    std::vector<cv::Rect>    detect_face_rects_;

};


#endif //JKLIBS_CVFACEDETECT_H
