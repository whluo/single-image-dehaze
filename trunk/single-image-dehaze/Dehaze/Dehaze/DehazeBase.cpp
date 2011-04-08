#include "DehazeBase.h"
#include "Set.h"
#include "DehazeTools.h"

DehazeBase::DehazeBase()
:m_DarkestImage(NULL),
 m_TranMapImage(NULL),
 m_SmoothImage(NULL),
 m_BilaFilImage(NULL),
 m_Result(NULL)
{}

DehazeBase::~DehazeBase()
{
	if (m_DarkestImage != NULL)
		cvReleaseImage(&m_DarkestImage);
	if (m_TranMapImage != NULL)
		cvReleaseImage(&m_TranMapImage);
	if (m_SmoothImage != NULL)
		cvReleaseImage(&m_SmoothImage);
	if (m_BilaFilImage != NULL)
		cvReleaseImage(&m_BilaFilImage);
}


void DehazeBase::setMedianParam(unsigned int Median1_Winsize, unsigned int Median2_Winsize)
{
	m_Median1_Winsize = Median1_Winsize;
	m_Median2_Winsize = Median2_Winsize;
}

void DehazeBase::setBilateralParam(unsigned int Bilateral_Winsize, double Bilateral_Dr,
		double Bilateral_Dd, unsigned int Bilateral_BinNum)
{
	m_Bilateral_Winsize = Bilateral_Winsize;
	m_Bilateral_Dr = Bilateral_Dr;
	m_Bilateral_Dd = Bilateral_Dd;
	m_Bilateral_BinNum = Bilateral_BinNum;
}

void DehazeBase::setJointParam(unsigned int Joint_WinSize, double Joint_DrL, double Joint_DrC,
		double Joint_DdL, unsigned int Joint_BinNum)
{
	m_Joint_Winsize = Joint_WinSize;
	m_Joint_DrL = Joint_DrL;
	m_Joint_DrC = Joint_DrC;
	m_Joint_DdL = Joint_DdL;
	m_Joint_BinNum = Joint_BinNum;
}

void DehazeBase::ReleaseAll()
{
	if (m_DarkestImage != NULL)
		cvReleaseImage(&m_DarkestImage);
	if (m_TranMapImage != NULL)
		cvReleaseImage(&m_TranMapImage);
	if (m_SmoothImage != NULL)
		cvReleaseImage(&m_SmoothImage);
	if (m_BilaFilImage != NULL)
		cvReleaseImage(&m_BilaFilImage);
}

IplImage *DehazeBase::doDehaze(const IplImage *src)
{
	int airlight;
	double *TranMap  = new double[src->height * src->width];

	IplImage *temp = cvCreateImage( cvGetSize(src), src->depth, 1 );

	m_Result = cvCreateImage( cvGetSize(src), src->depth, src->nChannels );
	m_DarkestImage = cvCreateImage( cvGetSize(src), src->depth, 1 );
	m_TranMapImage = cvCreateImage( cvGetSize(src), src->depth, 1 );
	m_SmoothImage  = cvCreateImage( cvGetSize(src), src->depth, 1 );
	m_BilaFilImage = cvCreateImage( cvGetSize(src), src->depth, 1 );

	// The input is m_DarkestImage
	generateDarkestImage(src);
	// The input is m_DarkestImage
	airlight = generateAirligth();

	// The output is in m_SmoothImage and the input is m_DarkestImage
	doSmooth();

	// The input is m_DarkestImage
	preDoEnhance(temp);
	// The input is m_SmoothImage and the output is in m_BilaFilImage
	doEnhance(temp);
	// doEnhance(m_DarkestImage);

	// The input is m_BilaFilImage
	generateTranMap(airlight, TranMap);
	// The output is in m_TranMapImage
	generateTranImage(TranMap);

	// The output is in m_Result
	doDehaze(src, TranMap, airlight);
	
	delete TranMap;
	cvReleaseImage(&temp);

	return m_Result;
}

int DehazeBase::generateAirligth()
{
	Histogram hist;
	hist.setzero();
	unsigned int Height = m_DarkestImage->height;
	unsigned int Width  = m_DarkestImage->width;

	for (unsigned int y = 0; y < Height; y++)
	{
		uchar *ptr = (uchar *) (m_DarkestImage->imageData + y * m_DarkestImage->widthStep);
		for (unsigned int x = 0; x < Width; x++)
		{
			hist.flush((int) ptr[x], 1);
		}
	}

	return hist.GetValueSN( (int) (Height * Width * AIR) );
}

void DehazeBase::generateTranMap(int airlight, double *TranMap )
{
	unsigned int Height = m_BilaFilImage->height;
	unsigned int Width  = m_BilaFilImage->width;

	for (unsigned int y = 0; y < Height; y++)
	{
		uchar *ptr = (uchar *) (m_BilaFilImage->imageData + y * m_BilaFilImage->widthStep);
		for (unsigned int x = 0; x < Width; x++)
		{
			TranMap[y * Width + x] = (double) (1.0 - OMG *  (double) ptr[x] / airlight );
			TranMap[y * Width + x] = max(TranMap[y * Width + x], 0.1);
		}
	}
}

void DehazeBase::doSmooth()
{
	unsigned int Height = m_DarkestImage->height;
	unsigned int Width  = m_DarkestImage->width;
	IplImage *temp1  = cvCreateImage( cvGetSize(m_DarkestImage), m_DarkestImage->depth, 1 );
	IplImage *temp2  = cvCreateImage( cvGetSize(m_DarkestImage), m_DarkestImage->depth, 1 );
	HistMedianFilter Filter( m_DarkestImage, m_Median1_Winsize, m_SmoothImage );
	Filter.HMedianFilter();

	doImageSubtract( m_DarkestImage, m_SmoothImage, temp1 );

	Filter.setimage(temp1);
	Filter.setresult(temp2);
	Filter.setwinsize(m_Median2_Winsize);
	Filter.HMedianFilter();

	doImageSubtract( m_SmoothImage, temp2, m_SmoothImage );

	for (unsigned int y = 0; y < Height; y++)
	{
		uchar *ptrdi = (uchar *) (m_DarkestImage->imageData + y * m_DarkestImage->widthStep);
		uchar *ptrsi = (uchar *) (m_SmoothImage->imageData + y * m_SmoothImage->widthStep);
		for (unsigned int x = 0; x < Width; x++)
		{
			ptrsi[x] = (uchar) (max(min(P * ptrsi[x], ptrdi[x]), 0));
		}
	}
	cvReleaseImage(&temp1);
	cvReleaseImage(&temp2);
}

void DehazeBase::generateDarkestImage(const IplImage *src)
{
	unsigned int Height = src->height;
	unsigned int Width  = src->width;

	for (unsigned int y = 0; y < Height; y++)
	{
		uchar *ptrs = (uchar *) (src->imageData + y * src->widthStep);
		uchar *ptrd = (uchar *) (m_DarkestImage->imageData + y * m_DarkestImage->widthStep);
		for (unsigned int x = 0; x < Width; x++)
		{
			ptrd[x] = (uchar) 255;
			for (int i = 0; i < src->nChannels; i++)
			{
				if (ptrs[src->nChannels * x + i] < ptrd[x])
					ptrd[x] = ptrs[src->nChannels * x + i];
			}
		}
	}
}

void DehazeBase::doImageSubtract(const IplImage *src1, const IplImage *src2, IplImage *result)
{
	unsigned int Height = src1->height;
	unsigned int Width  = src1->width;

	for (unsigned int y = 0; y < Height; y++)
	{
		uchar *ptrs1 = (uchar *) (src1->imageData + y * src1->widthStep);
		uchar *ptrs2 = (uchar *) (src2->imageData + y * src2->widthStep);
		uchar *ptr = (uchar *) (result->imageData + y * result->widthStep);
		for (unsigned int x = 0; x < Width; x++)
		{
			ptr[x] = abs(ptrs1[x] - ptrs2[x]);
		}
	}
}

void DehazeBase::generateTranImage(double *TranMap)
{
	unsigned int Height = m_TranMapImage->height;
	unsigned int Width  = m_TranMapImage->width;

	for (unsigned int y = 0; y < Height; y++)
	{
		uchar *ptr = (uchar *) (m_TranMapImage->imageData + y * m_TranMapImage->widthStep);
		for (unsigned int x = 0; x < Width; x++)
		{
			ptr[x] = (uchar) (TranMap[y * Width + x] * 255.0);
		}
	}
}

void DehazeBase::doDehaze(const IplImage *src, double *TranMap, int airlight)
{
	unsigned int Height = src->height;
	unsigned int Width  = src->width;

	for (unsigned int y = 0; y < Height; y++)
	{
		uchar *ptrr = (uchar *) (m_Result->imageData + y * m_Result->widthStep);
		uchar *ptrs = (uchar *) (src->imageData + y * src->widthStep);
		for (unsigned int x = 0; x < Width; x++)
		{
			for (int i = 0; i < src->nChannels; i++)
			{
				double tmpvar = 
					airlight + (double) ((ptrs[src->nChannels * x + i] - airlight) / TranMap[y * Width + x]);
				if (tmpvar > 255.0)
					tmpvar = 255;
				if (tmpvar < 0.0)
					tmpvar = 0;
				ptrr[m_Result->nChannels * x + i] = (uchar) tmpvar;
			}
		}
	}
}
