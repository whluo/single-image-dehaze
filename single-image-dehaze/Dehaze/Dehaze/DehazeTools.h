#ifndef DEHAZE_TOOLS_H
#define DEHAZE_TOOLS_H

#include "cv.h"
#include "highgui.h"

class Histogram;
class HistMedianFilter;

class common_GuidedBilateral_Filter;
class fast_GuidedBilateralFilter;
class fast_BilateralFilter;
class common_BilateralFilter;

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
	unsigned int m_KernelSize;
	double m_Dr;
	double m_Dd;
	IplImage *m_Result;
};

class fast_BilateralFilter
{
public:
	// Constructors and deconstructor
	fast_BilateralFilter();
	fast_BilateralFilter( const unsigned int KernelSize,
		const unsigned int BinNum,
		const double DrL,
		const double DdL );
	
	void SetKernelSize( const unsigned int KernelSize );
	void SetBinNum( const unsigned int BinNum );
	void SetDrL( const double DrL );
	void SetDdL( const double DdL );
	void doBilateralFilter( const IplImage *SrcImageL, IplImage *result );

protected:
	unsigned int m_KernelSize;
	unsigned int m_BinNum;
	unsigned int m_ImageHeight;
	unsigned int m_ImageWidth;
	// Gauss range variance on the local image
	double m_DrL;
	// Gauss spatial variance 
	double m_DdL;
	// The result image 
	IplImage *m_ResultImg;

};

class fast_GuidedBilateralFilter
{
public:
	// Constructors and deconstructor
	fast_GuidedBilateralFilter();
	fast_GuidedBilateralFilter( const unsigned int KernelSize,
		const unsigned int BinNum,
		const double DrC,
		const double DrL,
		const double DdL);
	
	void SetKernelSize( const unsigned int KernelSize );
	void SetBinNum( const unsigned int BinNum );
	void SetDrC( const double DrC );
	void SetDrL( const double DrL );
	void SetDdL( const double DdL );
	void doGuidedBilateralFilter( const IplImage *SrcImageL, const IplImage *SrcImageR, IplImage *result );

protected:
	unsigned int m_KernelSize;
	unsigned int m_BinNum;
	unsigned int m_ImageHeight;
	unsigned int m_ImageWidth;
	// Gauss range variance between the pixels on the same position of the two source images
	double m_DrC;
	// Gauss range variance on the local image
	double m_DrL;
	// Gauss spatial variance 
	double m_DdL;
	// The result image 
	IplImage *m_ResultImg;

};

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
	double m_DrL;
	double m_DrC;
	double m_DdL;
	unsigned int m_KernelSize;

	IplImage *m_Result;
};

class Histogram
{
public:
	Histogram(void);
	Histogram(const Histogram & hist);

public:
	int getNth(int n) const ;
	void setzero();
	Histogram & operator = (Histogram & hist);
	Histogram  operator + (const Histogram & hist);
	Histogram  operator - (const Histogram & hist);
	int GetValueSN(int n);
	void flush(int i,int flag);
	void flushn(int i,int n,int flag);

private:
	int m_h[256];
};

class HistMedianFilter
{
public:
	HistMedianFilter(void);
	HistMedianFilter(IplImage * src,int winsize,IplImage * result);

public:
	void setimage(IplImage *src);
	void sethist(Histogram hist);
	void setwinsize(int winsize);
	void setresult(IplImage *result);

	IplImage * HMedianFilter();

private:

	IplImage *m_src;
	IplImage *m_result;
	Histogram m_hist;  
	int m_winsize;  
};

namespace fast
{
	class BilateralFilter : public fast_BilateralFilter 
	{
	public:
		BilateralFilter();
		BilateralFilter( const unsigned int KernelSize,
			const unsigned int BinNum,
			const double DrL,
			const double DdL);
	};

	class GuidedBilateralFilter : public fast_GuidedBilateralFilter 
	{
	public:
		GuidedBilateralFilter();
		GuidedBilateralFilter( const unsigned int KernelSize,
			const unsigned int BinNum,
			const double DrC,
			const double DrL,
			const double DdL);
	};
};

namespace common
{
	class BilateralFilter : public common_BilateralFilter 
	{
	public:
		BilateralFilter();
		BilateralFilter(unsigned int KernelSize, double Dr, double Dd);
	};

	class GuidedBilateralFilter : public common_GuidedBilateral_Filter 
	{
	public:
		GuidedBilateralFilter();
		GuidedBilateralFilter(unsigned int KernelSize, double DrL, 
			double DrC, double DdL);
	};
};

#endif
