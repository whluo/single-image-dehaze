#ifndef BILATERAL_FILTER_H
#define BILATERAL_FILTER_H

#include "cv.h"
#include "highgui.h"

class common_BilateralFilter
{
public:
	common_BilateralFilter();
	common_BilateralFilter(unsigned int KernelSize, double Dr, double Dd);

	void setDr(double Dr);
	void setDd(double Dd);
	void setKernelSize(unsigned int KernelSize);

	void doBilateralFilter(const IplImage *src, IplImage *result);

protected:
	void getDistParam(double *buf);
	void getImageContent(const IplImage *src, double *buf);
	void getResult(const double *DistParam, const double *ImageContent, CvSize size);

protected:
	unsigned int m_KernelSize;
	double m_Dr;
	double m_Dd;
	IplImage *m_Result;
};

namespace common
{
	class BilateralFilter : public common_BilateralFilter 
	{
	public:
		BilateralFilter() ;
		BilateralFilter(unsigned int KernelSize, double Dr, double Dd);
	};
};


#endif