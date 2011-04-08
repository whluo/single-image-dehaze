#include "FastGuidedBilateralFilter.h"
#include <math.h>

#define PID 3.1415926

fast_GuidedBilateralFilter::fast_GuidedBilateralFilter() 
:m_KernelSize(0),
 m_BinNum(0),
 m_DrC(0.0),
 m_DrL(0.0),
 m_DdL(0.0),
 m_ResultImg(NULL)
{
}

fast_GuidedBilateralFilter::fast_GuidedBilateralFilter( const unsigned int KernelSize, const unsigned int BinNum, 
					    const double DrC, const double DrL, 
						const double DdL) 
:m_KernelSize(KernelSize),
 m_BinNum(BinNum),
 m_DrC(DrC),
 m_DrL(DrL),
 m_DdL(DdL),
 m_ResultImg(NULL)
{
}

void fast_GuidedBilateralFilter::SetKernelSize( const unsigned int KernelSize )
{
	m_KernelSize = KernelSize;
}

void fast_GuidedBilateralFilter::SetBinNum( const unsigned int BinNum )
{
	m_BinNum = BinNum;
}

void fast_GuidedBilateralFilter::SetDrC( const double DrC )
{
	m_DrC = DrC;
}

void fast_GuidedBilateralFilter::SetDrL( const double DrL )
{
	m_DrL = DrL;
}

void fast_GuidedBilateralFilter::SetDdL( const double DdL )
{
	m_DdL = DdL;
}

void fast_GuidedBilateralFilter::doGuidedBilateralFilter( const IplImage *SrcImageL, const IplImage *SrcImageR, IplImage *result )
{
	m_ResultImg = result;
	m_ImageHeight = m_ResultImg->height;
	m_ImageWidth  = m_ResultImg->width;

	unsigned int temp = m_KernelSize / 2 + 1;

	double *UnitedImageL  = new double[m_ImageHeight * m_ImageWidth];
	double *UnitedImageR  = new double[m_ImageHeight * m_ImageWidth];
	double *UnitedResult  = new double[m_ImageHeight * m_ImageWidth];
	double *ParamCompare  = new double[m_ImageHeight * m_ImageWidth];
	double *ParamDistance = new double[temp * temp];
	double *Wk  = new double[m_ImageHeight * m_ImageWidth * (m_BinNum + 1)];
	double *Jk  = new double[m_ImageHeight * m_ImageWidth * (m_BinNum + 1)];
	double *JkB = new double[m_ImageHeight * m_ImageWidth * (m_BinNum + 1)];

	Unit( SrcImageL, UnitedImageL );
	Unit( SrcImageR, UnitedImageR );

	GetParamCompare( UnitedImageL, UnitedImageR, ParamCompare );
	GetParamDistance( ParamDistance );
	GetWkJk( UnitedImageL, UnitedImageR, Wk, Jk );
	delete UnitedImageR;
	GetJkB( Wk, Jk, ParamCompare, ParamDistance, JkB );
	delete ParamCompare;
	delete ParamDistance;
	delete Wk;
	delete Jk;
	GetUnitedResult( UnitedImageL, JkB, UnitedResult );
	delete UnitedImageL;
	delete JkB;
	ExtractImage( UnitedResult );
}

void fast_GuidedBilateralFilter::GetUnitedResult( const double *UnitedImage, const double *JkB, double *UnitedResult )
{
	double BinStep = 256.0 / m_BinNum;
	unsigned long ImageSize = m_ImageHeight * m_ImageWidth;
	for( unsigned int y = 0; y < m_ImageHeight; y++ )
	{
		for( unsigned int x = 0; x < m_ImageWidth; x++ )
		{
			unsigned long CurrentPosition = y * m_ImageWidth + x;
			double CurrentRange = UnitedImage[CurrentPosition];
			unsigned int Bin = (unsigned int) (CurrentRange / BinStep);
			while( Bin >= m_BinNum )
			{
				Bin--;
			}
			UnitedResult[CurrentPosition] = (Bin * BinStep - CurrentRange + BinStep) * JkB[Bin * ImageSize + CurrentPosition ] +
				(CurrentRange - Bin * BinStep) * JkB[Bin * ImageSize + CurrentPosition + ImageSize];
			UnitedResult[CurrentPosition] /= BinStep;
		}
	}
}

void fast_GuidedBilateralFilter::Unit( const IplImage *SrcImage, double *UnitedImage )
{
	for( unsigned int y = 0; y < m_ImageHeight; y++ )
	{
		uchar *ptr = (uchar *) ( SrcImage->imageData + y * SrcImage->widthStep );
		for( unsigned int x = 0; x < m_ImageWidth; x++ )
		{
			UnitedImage[y * m_ImageWidth + x] = (double) (ptr[x] /*/ 255.0*/);
		}
	}
}

void fast_GuidedBilateralFilter::ExtractImage( const double *UnitedImage )
{
	for( unsigned int y = 0; y < m_ImageHeight; y++ )
	{
		uchar *ptr = (uchar *) ( m_ResultImg->imageData + y * m_ResultImg->widthStep );
		for( unsigned int x = 0; x < m_ImageWidth; x++ )
		{
			ptr[x] = (uchar) (UnitedImage[y * m_ImageWidth + x] /** 255*/);
		}
	}
}

void fast_GuidedBilateralFilter::GetParamCompare( const double *UnitedImageL, const double *UnitedImageR, double *ParamCompare )
{
	for( unsigned int y = 0; y < m_ImageHeight; y++ )
	{
		for( unsigned int x = 0; x < m_ImageWidth; x++ )
		{
			unsigned long CurrentPosition = y * m_ImageWidth + x;
			double temp = UnitedImageL[CurrentPosition] - UnitedImageR[CurrentPosition];
			double expvar  = - ((temp * temp) / (2.0 * m_DrC * m_DrC));
			double sqrtvar = 2.0 * PID * m_DrC * m_DrC;
			ParamCompare[CurrentPosition] = exp(expvar) / sqrt(sqrtvar);
		}
	}
}

void fast_GuidedBilateralFilter::GetParamDistance( double *ParamDistance )
{
	unsigned int temp = m_KernelSize / 2 + 1;
	for( unsigned int y = 0; y < temp; y++ )
	{
		for( unsigned int x = 0; x < temp; x++)
		{
			double Distance = x * x + y * y;
			double expvar  = -(Distance / (2.0 * m_DdL * m_DdL));
			double sqrtvar = 2.0 * PID * m_DdL * m_DdL;
			ParamDistance[y * temp + x] = exp(expvar) / sqrt(sqrtvar);
		}
	}
}

void fast_GuidedBilateralFilter::GetWkJk( const double *UnitedImageL, const double *UnitedImageR, double *Wk, double *Jk )
{
	unsigned long ImageSize = (unsigned long) ( m_ImageHeight * m_ImageWidth );
	double BinStep = 256.0 / m_BinNum;
	for( unsigned int k = 0; k <= m_BinNum; k++ )
	{
		double CurrentRange = k * BinStep;
		for( unsigned int y = 0; y < m_ImageHeight; y++ )
		{
			for( unsigned int x = 0; x < m_ImageWidth; x++ )
			{
				unsigned long CurrentPosition = k * ImageSize + y * m_ImageWidth + x;
				unsigned long CurrentPixel = y * m_ImageWidth + x;
				double temp = UnitedImageR[CurrentPixel] - CurrentRange;
				double expvar  = -((temp * temp) / (2.0 * m_DrL * m_DrL));
				double sqrtvar = 2.0 * PID * m_DrL * m_DrL;
				Wk[CurrentPosition] = exp(expvar) / sqrt(sqrtvar);
				Jk[CurrentPosition] = UnitedImageL[CurrentPixel] * Wk[CurrentPosition];
			}
		}
	}
}

void fast_GuidedBilateralFilter::GetJkB( const double *Wk, const double *Jk, const double *ParamCompare, const double *ParamDistance, double *JkB )
{
	double SumW;
	double SumJ;

	CPD Param[4];
	Param[0] = Wk;
	Param[1] = Jk;
	Param[2] = ParamCompare;
	Param[3] = ParamDistance;

	int Height = (int) m_ImageHeight;
	int Width  = (int) m_ImageWidth;
	int BinNum = (int) m_BinNum;
	long ImageSize = (long) Height * Width;
	int bias = m_KernelSize / 2;
	int x, y;
	int bx, by;
	int flag;
	long CurrentPosition;

	for( int k = 0; k <= BinNum; k++ )
	{
		// initial
		x = y = 0;
		SumW = SumJ = 0;
		flag = MOVE_DOWN;
		CurrentPosition = k * ImageSize;

		// At the pixel ( 0, 0 )
		for( by = 0; by <= bias; by++ )
		{
			for( bx = 0; bx <= bias; bx++ )
			{
				long temp1 = k * ImageSize + by * Width + bx;
				long temp2 = by * Width + bx;
				long temp3 = by * (bias + 1) + bx;
				SumW += ParamCompare[temp2] * ParamDistance[temp3] * Wk[temp1];
				SumJ += ParamCompare[temp2] * ParamDistance[temp3] * Jk[temp1];
			}
		}
		JkB[CurrentPosition] = SumJ / SumW;

		while( x < Width )
		{
			switch( flag )
			{
			// Down
			case MOVE_DOWN:
				// Subtract the old line
				DealMove( k, x, y - bias, SumW, SumJ, Param, Y_MOVE_SUB );
				y++;
				CurrentPosition += Width;
				DealMove( k, x, y + bias, SumW, SumJ, Param, Y_MOVE_ADD );
				break;
			// Up
			case MOVE_UP:
				// Subtract the old line
				DealMove( k, x, y + bias, SumW, SumJ, Param, Y_MOVE_SUB );
				y--;
				CurrentPosition -= Width;
				DealMove( k, x, y - bias, SumW, SumJ, Param, Y_MOVE_ADD );
				break;
			}
			JkB[CurrentPosition] = SumJ /SumW;

			if( (y == 0) || (y == Height - 1) )
			{
				DealMove( k, x - bias, y, SumW, SumJ, Param, X_MOVE_SUB );
				x++;
				CurrentPosition++;
				DealMove( k, x + bias, y, SumW, SumJ, Param, X_MOVE_ADD );
				JkB[CurrentPosition] = SumJ /SumW;
				flag = 1 - flag;
			}
		}
	}
}

void fast_GuidedBilateralFilter::DealMove( int k, int x, int y, double &SumW, double &SumJ, CPD *Param, int flag )
{
	int Height = (int) m_ImageHeight;
	int Width  = (int) m_ImageWidth;
	long ImageSize = (long) (Height * Width);
	int bias   = (int) (m_KernelSize / 2);
	long temp1, temp2, temp3;
	bool IsLegal;

	for( int b = -bias; b <= bias; b++ )
	{
		IsLegal = false;
		switch( flag )
		{
		case Y_MOVE_ADD:
		case Y_MOVE_SUB:
			if( (y >= 0) && (y < Height) && ((b + x) >= 0) && ((b + x) < Width) )
			{
				temp1 = k * ImageSize + y * Width + b + x;
				temp2 = y * Width + b + x;
				temp3 = bias * (bias + 1) + abs(b);
				IsLegal = true;
			}
			break;
		case X_MOVE_ADD:
		case X_MOVE_SUB:
			if( (x >= 0) && (x < Width) && ((b + y) >= 0) && ((b + y) < Height) )
			{
				temp1 = k * ImageSize + (y + b) * Width + x;
				temp2 = (y + b) * Width + x;
				temp3 = abs(b) * (bias + 1) + bias;
				IsLegal = true;
			}
			break;
		}
		if( IsLegal )
		{
			switch( flag )
			{
			case Y_MOVE_ADD:
			case X_MOVE_ADD:
				SumW += Param[2][temp2] * Param[3][temp3] * Param[0][temp1];
				SumJ += Param[2][temp2] * Param[3][temp3] * Param[1][temp1];
				break;
			case Y_MOVE_SUB:
			case X_MOVE_SUB:
				SumW -= Param[2][temp2] * Param[3][temp3] * Param[0][temp1];
				SumJ -= Param[2][temp2] * Param[3][temp3] * Param[1][temp1];
				break;
			}
		}
	}
}

fast::GuidedBilateralFilter::GuidedBilateralFilter() 
{};

fast::GuidedBilateralFilter::GuidedBilateralFilter( const unsigned int KernelSize,
	const unsigned int BinNum,
	const double DrC,
	const double DrL,
	const double DdL)
	:fast_GuidedBilateralFilter(KernelSize, BinNum, DrC, DrL, DdL)
{};