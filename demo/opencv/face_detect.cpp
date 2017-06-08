//
// Created by jmdvirus on 2017/6/8.
//

#include "CVFaceDetect.h"


int main(int argc, char **args) {
    const char *img = args[1];

    CVFaceDetect fd;
    fd.face_detect_draw_image(img);

    return 0;
}
