//
// Created by jmdvirus on 2017/6/8.
//

#include<cstring>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <stdio.h>

using namespace std;
using namespace cv;

double tmp;

int main(int argc, char **argv){
    Mat frame;
    bool stop(false);

    VideoCapture capture(argv[1]);
    if (!capture.isOpened()){
        return 0;
    }

    while(!stop){
        if (!capture.read(frame)){
            break;
        }
        // show foreground
        imshow("Video", frame);
        if (waitKey(50) == 'q'){
            stop = true;
        }
    }

    return 0;
}