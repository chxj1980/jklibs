//
// Created by jmdvirus on 2017/6/8.
//

#ifndef JKLIBS_CVFACEDETECT_H
#define JKLIBS_CVFACEDETECT_H

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"

using namespace cv;
using namespace std;

class CVFaceDetect {
public:
    std::vector<Rect> detect_face(const char *buffer, int len, Size size);
    std::vector<Rect> detect_face_image(const char *filename);

    // Debug for view
    int face_detect_draw_image(const char *filename);

protected:
    std::vector<Rect> detectFaces(Mat img_gray);

};


#endif //JKLIBS_CVFACEDETECT_H
