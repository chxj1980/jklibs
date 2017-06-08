//
// Created by jmdvirus on 2017/6/8.
//

#include "opencv2/highgui/highgui.hpp"
int gSliderPos = 0;
CvCapture* gCapture = NULL;
void showPosFrame(int pos) {
    /*show the pos of frame*/
    cvSetCaptureProperty(gCapture, CV_CAP_PROP_POS_FRAMES, pos);
}
int main(int argc, char ** argv) {
    char* fileName = argv[1];
    char* windowTitle = (char*)"video";
    int ESC_KEY = 27;
    cvNamedWindow(windowTitle, CV_WINDOW_AUTOSIZE);
    CvCapture *capture = cvCreateFileCapture(fileName);
    gCapture = capture;
    int frameCount = (int) cvGetCaptureProperty(capture,
                                                CV_CAP_PROP_FRAME_COUNT);
    if (frameCount != 0) {
        cvCreateTrackbar("Track Bar", windowTitle, &gSliderPos, frameCount,
                         showPosFrame);
    }
    IplImage* frame;
    while (1) {
        frame = cvQueryFrame(capture);
        if (!frame) {
            break;
        }
        cvShowImage(windowTitle, frame);
        gSliderPos++;
        if (gSliderPos % 150 == 0)
            cvSetTrackbarPos("Track Bar", windowTitle, gSliderPos);
        int c = cvWaitKey(50);
        if (c == ESC_KEY) {
            break;
        }
    }
    cvReleaseCapture(&capture);
    cvDestroyWindow(windowTitle);
    return 0;
}