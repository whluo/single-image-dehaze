#include "GuidedBilateralFilter.h"

common_GuidedBilateral_Filter::common_GuidedBilateral_Filter()
:m_Result(NULL)
{}

common_GuidedBilateral_Filter::common_GuidedBilateral_Filter( unsigned int KernelSize,
								double DrL, double DrC, double DdL)
:m_Result(NULL),
 m_KernelSize(KernelSize),
 m_DrL(DrL),
 m_DrC(DrC),
 m_DdL(DdL)
{}

void common_GuidedBilateral_Filter::setDrL(double DrL)
{
	m_DrL = DrL;
}

void common_GuidedBilateral_Filter::setDrC(double DrC)
{
	m_DrC = DrC;
}

void common_GuidedBilateral_Filter::setDdL(double DdL)
{
	m_DdL = DdL;
}

void common_GuidedBilateral_Filter::setKernelSize(unsigned int KernelSize)
{
	m_KernelSize = KernelSize;
}


void common_GuidedBilateral_Filter::doGuidedBilateralFilter(const IplImage *src, const IplImage *ref, IplImage *result)
{
	m_Result = result;

	int temp = m_KernelSize / 2 + 1;
	double *srcContent = new double[src->height * src->width];
	double *refContent = new double[ref->height * src->width];
	double *DistanceParam = new double[temp * temp];
	double *CompareParam  = new double[src->height * src->width];

	getImageContent(src, srcContent);
	getImageContent(ref, refContent);
	getDistParam(DistanceParam);
	getCompareParam(srcContent, refContent, CompareParam, cvGetSize(src));
	getResult(CompareParam, DistanceParam, srcContent, refContent, cvGetSize(src));

	delete srcContent;
	delete refContent;
	delete DistanceParam;
	delete CompareParam;
}

void common_GuidedBilateral_Filter::getDistParam(double *buf)
{
	unsigned int DisArr = m_KernelSize / 2 + 1;
	for (unsigned int y = 0; y < DisArr; y++)
	{
		for (unsigned int x = 0; x < DisArr; x++)
		{
			double temp = x * x + y * y;
			double expvar = -(temp / (2 * m_DdL * m_DdL));
			buf[y * DisArr + x] = exp(expvar);
		}
	}
}

void common_GuidedBilateral_Filter::getImageContent(const IplImage *src, double *buf)
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

void common_GuidedBilateral_Filter::getCompareParam(const double *src, 
					const double *ref, double *ComParam, CvSize size)
{
	unsigned int Height = size.height;
	unsigned int Width  = size.width;

	for (unsigned int y = 0; y < Height; y++)
	{
		for (unsigned int x = 0; x < Width; x++)
		{
			unsigned long CurrentPos = y * Width + x;
			double expvar = -(((src[CurrentPos] - ref[CurrentPos]) *
				(src[CurrentPos] - ref[CurrentPos])) / (2 * m_DrC * m_DrC));
			ComParam[CurrentPos] = exp(expvar); 
		}
	}
}

void common_GuidedBilateral_Filter::getResult(const double *ComParam, const double *DisParam, 
							const double *ImageContentsrc, const double *ImageContentref, CvSize size)
{
	int bias = m_KernelSize / 2;
	unsigned int ImageHeight = size.height;
	unsigned int ImageWidth  = size.width;

	for (unsigned int y = 0; y < ImageHeight; y++)
	{
		uchar *ptr = (uchar *) (m_Result->imageData + y * m_Result->widthStep);
		for (unsigned int x = 0; x < ImageWidth; x++)
		{
			double CurrentRange = ImageContentref[y * ImageWidth + x];
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
						double RangeExpvar = -((ImageContentref[CurrentPos] - CurrentRange)
							* (ImageContentref[CurrentPos] - CurrentRange) / (2 * m_DrL * m_DrL));
						Weight += exp(RangeExpvar) * DisParam[CurrentDisP] * ComParam[CurrentPos];
						Sum += exp(RangeExpvar) * DisParam[CurrentDisP] * 
							ComParam[CurrentPos] * ImageContentsrc[CurrentPos];
 					}
				}
			}

			ptr[x] = (uchar) (Sum / Weight);
		}
	}
}

common::GuidedBilateralFilter::GuidedBilateralFilter()
{}

common::GuidedBilateralFilter::GuidedBilateralFilter(unsigned int KernelSize, double DrL, double DrC, double DdL)
:common_GuidedBilateral_Filter(KernelSize, DrL, DrC, DdL)
{}