//
// Created by jmdvirus on 2017/6/8.
//

#include "CVFaceDetect.h"


#ifdef _WIN32
#define OPENCV_FONTFACE_PATH "./"
#elif __MACOS__
#define OPENCV_FONTFACE_PATH "/Users/jmdvirus/jmd/proj/app/source/opencv-3.2.0/data/haarcascades/"
#else
#define OPENCV_FONTFACE_PATH "/opt/Arges/etc/conf/haarcascades/"
//#define OPENCV_FONTFACE_PATH "/opt/Arges/conf/hogcascades/"
#endif
#define OPENCV_FONTFACE_FILE "haarcascade_frontalface_alt.xml"
//#define OPENCV_FONTFACE_FILE "hogcascade_pedestrians.xml"

/*---- detectFaces函数 -------
检测灰度图片中的人脸，返回人脸矩形坐标(x,y,width,height)
因为可能检测出多个人脸，所以返回类型为vetor<Rect>
------------------------------------------------------*/
std::vector<Rect> CVFaceDetect::detectFaces(Mat img_gray){
    CascadeClassifier faces_cascade;
    int ret = faces_cascade.load(OPENCV_FONTFACE_PATH OPENCV_FONTFACE_FILE);
    vector<Rect> faces;
    faces.clear();
    if (!ret) {
        printf("local frontface file failed [ %s ]", OPENCV_FONTFACE_PATH OPENCV_FONTFACE_FILE);
        return faces;
    }
    faces_cascade.detectMultiScale(img_gray,faces,1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );
    return faces;
}

std::vector<Rect> CVFaceDetect::detect_face(const char *buffer, int len, Size size) {
    std::vector<Rect> retRect;
    if (!buffer || len <= 0) return retRect;

    Mat l_image(size.height + size.width/2, size.width, CV_8UC1, (char*)buffer);
    Mat l_img_gray;
    cvtColor(l_image, l_img_gray, COLOR_YUV420p2GRAY);
    equalizeHist(l_img_gray, l_img_gray);
    vector<Rect> faces = detectFaces(l_img_gray);
    for (int i = 0; i < faces.size(); i++) {
        Rect roi = faces[i];
        retRect.push_back({roi.x, roi.y, roi.width, roi.height});
    }
    return retRect;
}

std::vector<Rect> CVFaceDetect::detect_face_image(const char *filename) {
    IplImage* image = cvLoadImage( filename, 1 );

    std::vector<Rect> retRect;
    if (!image) return retRect;

    Mat l_image = cvarrToMat(image, false);
    Mat l_img_gray;
    cvtColor(l_image, l_img_gray, COLOR_BGR2GRAY);
    equalizeHist(l_img_gray, l_img_gray);
    vector<Rect> faces = detectFaces(l_img_gray);
    for (int i = 0; i < faces.size(); i++) {
        Rect roi = faces[i];
        retRect.push_back({roi.x, roi.y, roi.width, roi.height});
    }
    cvReleaseImage(&image);
    return retRect;
}

int CVFaceDetect::face_detect_draw_image(const char *filename) {

    namedWindow("face detect");

    IplImage* image = cvLoadImage( filename, 1);

    static CvScalar colors = { 100, 100, 100};
    std::vector<Rect> ret = detect_face_image(filename);
    for (int i = 0; i < ret.size(); i++) {
        Rect dr = ret[i];
        CvRect cr(dr.x, dr.y, dr.width, dr.height);
        printf("Out index [%d] [%d, %d, %d, %d]\n", i, dr.x, dr.y, dr.width, dr.height);
        cvRectangleR(image, cr, colors);
    }
    cvShowImage("face detect", image);
    waitKey(0);
    cvReleaseImage(&image);
    cvDestroyWindow("face detect");

    return 0;
}