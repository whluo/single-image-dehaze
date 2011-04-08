#ifndef GUIDED_BILATERAL_FILTER_H
#define GUIDED_BILATERAL_FILTER_H

#include "cv.h"
#include "highgui.h"

class common_GuidedBilateral_Filter
{
public:
	common_GuidedBilateral_Filter();
	common_GuidedBilateral_Filter(unsigned int KernelSize, double DrL, 
		double DrC, double DdL);

	void setDrL(double DrL);
	void setDrC(double DrC);
	void setDdL(double DdL);
	void setKernelSize(unsigned int KernelSize);

	void doGuidedBilateralFilter(const IplImage *src, const IplImage *ref, IplImage *result);

protected:
	void getCompareParam(const double *src, const double *ref, 
			double *ComParam, CvSize size);
	void getImageContent(const IplImage *src, double *buf);
	void getDistParam(double *buf);
	void getResult(const double *ComParam, const double *DisParam, 
		const double *ImageContentsrc, const double *ImageContentref, CvSize size);
protected:
	double m_DrL;
	double m_DrC;
	double m_DdL;
	unsigned int m_KernelSize;

	IplImage *m_Result;
};

namespace common
{
	class GuidedBilateralFilter : public common_GuidedBilateral_Filter 
	{
	public:
		GuidedBilateralFilter() ;
		GuidedBilateralFilter(unsigned int KernelSize, double DrL, 
			double DrC, double DdL);
	};
}

#endif