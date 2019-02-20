
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/stitching.hpp"

#include "cm_logprint.h"
#include "cmyuv.h"

#include <iostream>

using namespace std;
using namespace cv;

CMLogPrint logPrint;

bool try_use_gpu = false;
bool divide_images = false;
Stitcher::Mode mode = Stitcher::PANORAMA;
vector<Mat> imgs;
string result_name = "result.jpg";

void printUsage(char** argv);
int parseCmdArgs(int argc, char** argv);

int main(int argc, char* argv[])
{
    int retval = parseCmdArgs(argc, argv);
    if (retval) return EXIT_FAILURE;

    Mat pano;
    Ptr<Stitcher> stitcher = Stitcher::create(mode);
    Stitcher::Status status = stitcher->stitch(imgs, pano);

    if (status != Stitcher::OK)
    {
        cout << "Can't stitch images, error code = " << int(status) << endl;
        return EXIT_FAILURE;
    }

    imwrite(result_name, pano);
    cout << "stitching completed successfully\n" << result_name << " saved!";
    return EXIT_SUCCESS;
}


void printUsage(char** argv)
{
    cout <<
         "Images stitcher.\n\n" << "Usage :\n" << argv[0] <<" [Flags] img1 img2 [...imgN]\n\n"
         "Flags:\n"
         "  --d3\n"
         "      internally creates three chunks of each image to increase stitching success"
         "  --try_use_gpu (yes|no)\n"
         "      Try to use GPU. The default value is 'no'. All default values\n"
         "      are for CPU mode.\n"
         "  --mode (panorama|scans)\n"
         "      Determines configuration of stitcher. The default is 'panorama',\n"
         "      mode suitable for creating photo panoramas. Option 'scans' is suitable\n"
         "      for stitching materials under affine transformation, such as scans.\n"
         "  --output <result_img>\n"
         "      The default is 'result.jpg'.\n\n"
         "Example usage :\n" << argv[0] << " --d3 --try_use_gpu yes --mode scans img1.jpg img2.jpg";
}

int read_yuv_mat(const char *filename, vector<Mat> &saveimgs)
{
	if (!filename) return -1;
	// yuv422
	int len = 640 * 480 * 2;
	char *data = (char *)calloc(1, len);
	// yuv420
	int dstlen = 640 * 480 * 3/2;
	char *dstdata = (char*)calloc(1, dstlen);

	int frames = 3;
	FILE *f = fopen(filename, "r");
	int skip_frames = 1;
	if (f) {
		while (frames) {
		    int ret = fread(data, 1, len, f);
		    if (ret != len) {
		    	printf("Read error of file \n");
				break;
		    }
			if (--skip_frames) continue;
			printf("Read out data of len %d\n", len);
			cm_yuy2_yuv420p(data, 640, 480, dstdata);
			printf("Convert done of len [%d]\n", dstlen);
			Mat yuvImgt, yuvImg;
			yuvImgt.create(480*3/2, 640, CV_8UC1);
			memcpy(yuvImgt.data, dstdata, dstlen);
			cvtColor(yuvImgt, yuvImg, cv::COLOR_YUV420p2RGB);
			char name[32] = {0};
			sprintf(name, "x-%d.jpg", frames);
			imwrite(name, yuvImg);
			saveimgs.push_back(yuvImg);
			skip_frames = 10;
			frames--;
		}

		fclose(f);
	}
	printf("get saveimgs count [%u]\n", saveimgs.size());

	return 0;
}

int parseCmdArgs(int argc, char** argv)
{
    if (argc == 1)
    {
        printUsage(argv);
        return -1;
    }

    for (int i = 1; i < argc; ++i)
    {
        if (string(argv[i]) == "--help" || string(argv[i]) == "/?")
        {
            printUsage(argv);
            return EXIT_FAILURE;
        }
        else if (string(argv[i]) == "--try_use_gpu")
        {
            if (string(argv[i + 1]) == "no")
                try_use_gpu = false;
            else if (string(argv[i + 1]) == "yes")
                try_use_gpu = true;
            else
            {
                cout << "Bad --try_use_gpu flag value\n";
                return -1;
            }
            i++;
        }
        else if (string(argv[i]) == "--d3")
        {
            divide_images = true;
        }
        else if (string(argv[i]) == "--output")
        {
            result_name = argv[i + 1];
            i++;
        }
        else if (string(argv[i]) == "--mode")
        {
            if (string(argv[i + 1]) == "panorama")
                mode = Stitcher::PANORAMA;
            else if (string(argv[i + 1]) == "scans")
                mode = Stitcher::SCANS;
            else
            {
                cout << "Bad --mode flag value\n";
                return EXIT_FAILURE;
            }
            i++;
        }
        else
        {
#if 1
			char *file = argv[i];
			read_yuv_mat(file, imgs);
#else
            Mat img = imread(argv[i]);
            if (img.empty())
            {
                cout << "Can't read image '" << argv[i] << "'\n";
                return EXIT_FAILURE;
            }

            if (divide_images)
            {
                Rect rect(0, 0, img.cols / 2, img.rows);
                imgs.push_back(img(rect).clone());
                rect.x = img.cols / 3;
                imgs.push_back(img(rect).clone());
                rect.x = img.cols / 2;
                imgs.push_back(img(rect).clone());
            }
            else
                imgs.push_back(img);
#endif
        }
    }
    return EXIT_SUCCESS;
}
