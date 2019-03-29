
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/stitching.hpp"

#include "cm_logprint.h"
#include "cmyuv.h"

#include <iostream>

using namespace std;
using namespace cv;
CMLogPrint logPrint;

bool divide_images = false;
Stitcher::Mode mode = Stitcher::PANORAMA;
vector<Mat> imgs;
string result_name = "result.jpg";

void printUsage(char** argv);
int parseCmdArgs(int argc, char** argv);

void yuyv_to_yuv420P(char *in, char*out,int width,int height) 
{
	char *p_in, *p_out, *y, *u, *v;
	int index_y, index_u, index_v;
	int i, j, in_len;
 
	y = out;
	u = out + (width * height);
	v = out + (width * height * 5/4);
 
	index_y = 0;
	index_u = 0;
	index_v = 0;
	for(j=0; j< height*2; j++)
	{
		for(i=0; i<width; i=i+4)
		{
			*(y + (index_y++)) = *(in + width * j + i);
			*(y + (index_y++)) = *(in + width * j + i + 2);
			if(j%2 == 0)
			{
				*(u + (index_u++)) = *(in + width * j + i + 1);
				*(v + (index_v++)) = *(in + width * j + i + 3);
			}
		}
	}
}

int stitch_whole_yuv(const char *file, int width, int height) 
{
	FILE *f = fopen(file, "r");
	if (!f) {
		printf("open error %s\n", file);
		return -1;
	}

	// yuyv
	int tmplen = width * height * 2;
	unsigned char *tmpdata = (unsigned char *)malloc(tmplen);

	// yuv
	int yuvlen = width * height * 3/2;
	unsigned char *yuvdata = (unsigned char *)malloc(yuvlen);

	Mat yuvImgt;
	yuvImgt.create(height*3/2, width, CV_8UC1);
    Mat pano;
    vector<Mat> saveimgs;

    Ptr<Stitcher> stitcher = Stitcher::create(mode);

	int imgcount = 0;
	int frames = 0;
	int first = 1;
	int ok = 0;
	char tmpf[32] = {0};
	while (1) {
		int ret = fread(tmpdata, 1, tmplen, f);
		if (ret <= 0) {
			if (ret == 0) {
				printf("Read done\n");
			} else {
				printf("Read error [%d]\n", errno);
			}

			break;
		}
		//yuyv_to_yuv420P((char*)tmpdata, (char*)yuvdata, width, height);
		cm_yuy2_yuv420p((const char*)tmpdata, width, height, (char*)yuvdata);

		Mat yuvImg;
		memcpy(yuvImgt.data, yuvdata, yuvlen*sizeof(unsigned char));
		cvtColor(yuvImgt, yuvImg, cv::COLOR_YUV420p2RGB);
		sprintf(tmpf, "o-%d.jpg", frames);
		imwrite(tmpf, yuvImg);
		saveimgs.push_back(yuvImg);
		printf("saveimgs.length= %d, %d\n", saveimgs.size(), ret);
        if (saveimgs.size() >= 2) {
			printf("start to stitch index [%d]\n", frames);
            Stitcher::Status status = stitcher->stitch(saveimgs, pano);
            if (status != Stitcher::OK) {
				printf("stitch error \n");
				if (saveimgs.size() >= 2) saveimgs.erase(saveimgs.begin()+1, saveimgs.end());
			} else {
				printf("stitch success [%d]\n", frames);
				first = 0;
				ok = 1;
			    saveimgs.clear();
				saveimgs.push_back(pano);
			}
			frames++;
		}
	}

	if (ok) {
	    imwrite("result.jpg", pano);
	}

	fclose(f);

	return 0;
}

int main(int argc, char* argv[])
{
	if (argv[1][0] == 'x') {
		const char *file = argv[2];
		int width = (int)strtol(argv[3], NULL, 10);
		int height = (int)strtol(argv[4], NULL, 10);
		printf("Do with [%s][%d,%d]\n", file, width, height);
        stitch_whole_yuv(file, width, height);
        //stitch_whole_yuv(file, 640, 480);
		return 0;
	}
    int retval = parseCmdArgs(argc, argv);
    if (retval) return EXIT_FAILURE;

    //![stitching]
    Mat pano;
    Ptr<Stitcher> stitcher = Stitcher::create(mode);
    Stitcher::Status status = stitcher->stitch(imgs, pano);

    if (status != Stitcher::OK)
    {
        cout << "Can't stitch images, error code = " << int(status) << endl;
        return EXIT_FAILURE;
    }
    //![stitching]

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
         "      internally creates three chunks of each image to increase stitching success\n"
         "  --mode (panorama|scans)\n"
         "      Determines configuration of stitcher. The default is 'panorama',\n"
         "      mode suitable for creating photo panoramas. Option 'scans' is suitable\n"
         "      for stitching materials under affine transformation, such as scans.\n"
         "  --output <result_img>\n"
         "      The default is 'result.jpg'.\n\n"
         "Example usage :\n" << argv[0] << " --d3 --try_use_gpu yes --mode scans img1.jpg img2.jpg\n";
}


int parseCmdArgs(int argc, char** argv)
{
    if (argc == 1)
    {
        printUsage(argv);
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; ++i)
    {
        if (string(argv[i]) == "--help" || string(argv[i]) == "/?")
        {
            printUsage(argv);
            return EXIT_FAILURE;
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
            Mat img = imread(samples::findFile(argv[i]));
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
        }
    }
    return EXIT_SUCCESS;
}
