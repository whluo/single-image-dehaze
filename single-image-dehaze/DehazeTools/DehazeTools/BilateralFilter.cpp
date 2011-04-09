#include "BilateralFilter.h"
#include <math.h>

#include <iostream>
using namespace std;
 
common_BilateralFilter::common_BilateralFilter()
:m_KernelSize(0),
 m_Dr(0.0),
 m_Dd(0.0),
 m_Result(NULL)
{}

common_BilateralFilter::common_BilateralFilter(unsigned int KernelSize, double Dr, double Dd)
:m_KernelSize(KernelSize),
 m_Dr(Dr),
 m_Dd(Dd),
 m_Result(NULL)
{}

void common_BilateralFilter::setDr(double Dr)
{
	m_Dr = Dr;
}

void common_BilateralFilter::setDd(double Dd)
{
	m_Dd = Dd;
}

void common_BilateralFilter::setKernelSize(unsigned int KernelSize)
{
	m_KernelSize = KernelSize;
}


void common_BilateralFilter::doBilateralFilter(const IplImage *src, IplImage *result)
{
	unsigned int ImageHeight = src->height;
	unsigned int ImageWidth  = src->width;
	unsigned int DisArr = m_KernelSize / 2 + 1;

	double *ImageContent  = new double[ImageHeight * ImageWidth];
	double *DistanceParam = new double[DisArr * DisArr];

	m_Result = result;

	// Calculate distance parameter
	getDistParam(DistanceParam);
	getImageContent(src, ImageContent);
	getResult(DistanceParam, ImageContent, cvGetSize(src));

	delete ImageContent;
	delete DistanceParam;
}

void common_BilateralFilter::getDistParam(double *buf)
{
	unsigned int DisArr = m_KernelSize / 2 + 1;
	for (unsigned int y = 0; y < DisArr; y++)
	{
		for (unsigned int x = 0; x < DisArr; x++)
		{
			double temp = x * x + y * y;
			double expvar = -(temp / (2 * m_Dd * m_Dd));
			buf[y * DisArr + x] = exp(expvar);
		}
	}
}

void common_BilateralFilter::getImageContent(const IplImage *src, double *buf)
{
	for (int y = 0; y < src->height; y++)
	{
		uchar *ptr = (uchar *) (src->imageData + y * src->widthStep);
		for (int x = 0; x < src->width; x++)
		{
			buf[y * src->width + x] = (double) ptr[x];
		}
	}
}

void common_BilateralFilter::getResult(const double *DistParam, const double *ImageContent, CvSize size)
{
	int bias = m_KernelSize / 2;
	unsigned int ImageHeight = size.height;
	unsigned int ImageWidth  = size.width;

	for (unsigned int y = 0; y < ImageHeight; y++)
	{
		uchar *ptr = (uchar *) (m_Result->imageData + y * m_Result->widthStep);
		for (unsigned int x = 0; x < ImageWidth; x++)
		{
			double CurrentRange = ImageContent[y * ImageWidth + x];
			double Weight, Sum;
			Weight = Sum = 0.0;

			for (int by = -bias; by <= bias; by++)
			{
				for (int bx = -bias; bx <= bias; bx++)
				{
					if (((y + by) >= 0) && ((y + by) < ImageHeight)
						&& ((x + bx) >= 0) && ((x + bx) < ImageWidth))
					{
						unsigned long CurrentPos = (y + by) * ImageWidth + x + bx;
						unsigned int CurrentDisP = abs(by) * (bias + 1) + abs(bx);
						double RangeExpvar = -((ImageContent[CurrentPos] - CurrentRange)
							* (ImageContent[CurrentPos] - CurrentRange) / (2 * m_Dr * m_Dr));
						Weight += exp(RangeExpvar) * DistParam[CurrentDisP];
						Sum += exp(RangeExpvar) * DistParam[CurrentDisP] * ImageContent[CurrentPos];
 					}
				}
			}

			ptr[x] = (uchar) (Sum / Weight);
		}
	}
}

common::BilateralFilter::BilateralFilter()
{}

common::BilateralFilter::BilateralFilter(unsigned int KernelSize, double Dr, double Dd)
:common_BilateralFilter(KernelSize, Dr, Dd)
{}