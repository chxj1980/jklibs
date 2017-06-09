//
// Created by jmdvirus on 2017/6/8.
//

#include "CVFaceDetect.h"


int main(int argc, char **args) {
    const char *img = args[1];

    CVFaceDetect fd;
    fd.face_detect_draw_image(img);

//    cv::Size size(1280, 720);
 //   fd.face_detect_draw_video(img, size);
    return 0;
}
