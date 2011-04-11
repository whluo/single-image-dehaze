#include "Set.h"
#include "FastDehaze.h"
#include "CommonDehaze.h"
#include "CorrosionDehaze.h"
#include <ctime>
#include <iostream>

using namespace std;

int main()
{
	IplImage *img = cvLoadImage(".//hongkong.bmp");
	IplImage *res;
	int winsize = 2 * max(img->height, img->width) / 50 + 1;
	winsize = WIN;
	CorrosionDehaze tool;
	tool.setMedianParam( winsize, winsize );
	tool.setBilateralParam( winsize, BDR, BDD, BBIN );
	tool.setJointParam( winsize, DRL, DRC, DD, BIN );
	unsigned long start = clock();
	res = tool.doDehaze(img);
	unsigned long end = clock();
	cvSaveImage("Result.bmp", res);
	res = tool.getTranMapImage();
	cvSaveImage("Result_T.bmp", res);
	res = tool.getSmoothImage();
	cvSaveImage("Smooth.bmp", res);
	res = tool.getBilaFilImage();
	cvSaveImage("BilaFil.bmp", res);
	cout << (end - start) / 1000 << endl; 
	system("pause");
	return 0;
}
