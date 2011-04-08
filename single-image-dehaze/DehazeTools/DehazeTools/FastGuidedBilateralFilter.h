#ifndef SUPER_JOINT_H
#define SUPER_JOINT_H

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
	void Unit( const IplImage *SrcImage, double *UnitedImage );
	void GetParamCompare( const double *UnitedImageL, const double *UnitedImageR, double *ParamCompare );
	void GetParamDistance( double *ParamDistance );
	void GetWkJk( const double *UnitedImageL, const double *UnitedImageR, double *Wk, double *Jk );;
	void GetJkB( const double *Wk, const double *Jk, const double *ParamCompare, const double *ParamDistance, double *JkB );
	void GetUnitedResult( const double *UnitedImage, const double *JkB, double *UnitedResult );
	void ExtractImage( const double *UnitedImage );
	void DealMove( int k, int x, int y, double &SumW, double &SumJ, CPD *Param, int flag );

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

namespace fast
{
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

#endif