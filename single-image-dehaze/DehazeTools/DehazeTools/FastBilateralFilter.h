#ifndef FAST_BILATERAL_FILTER_H
#define FAST_BILATERAL_FILTER_H

#define Y_MOVE_ADD 1
#define Y_MOVE_SUB -1
#define X_MOVE_ADD 2
#define X_MOVE_SUB -2

#define MOVE_DOWN 0
#define MOVE_UP	  1

#include "cv.h"
#include "highgui.h"

typedef unsigned char uchar;
typedef const double *CPD;

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
	void GetImageContent( const IplImage *SrcImage, double *UnitedImage );
	void GetParamDistance( double *ParamDistance );
	void GetWkJk( const double *UnitedImageL, double *Wk, double *Jk );;
	void GetJkB( const double *Wk, const double *Jk, const double *ParamDistance, double *JkB );
	void GetUnitedResult( const double *UnitedImage, const double *JkB, double *UnitedResult );
	void ExtractImage( const double *UnitedImage );
	void DealMove( int k, int x, int y, double &SumW, double &SumJ, CPD *Param, int flag );
	
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
}

#endif