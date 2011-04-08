#include "CorrosionDehaze.h"
#include "DehazeTools.h"
#include "Set.h"

void CorrosionDehaze::doSmooth()
{
	unsigned int Height = m_DarkestImage->height;
	unsigned int Width  = m_DarkestImage->width;
	int bias = (int) (WIN / 2);
	int flag = MOVE_DOWN;
	uchar *ptr = NULL;
	unsigned int y, x;
	int temp;
	Histogram hist;

	hist.setzero();
	x = y = 0;

	for (int by = 0; by <= bias; by++)
	{
		ptr = (uchar *) (m_DarkestImage->imageData + by * m_DarkestImage->widthStep);
		for (int bx = 0; bx <= bias; bx++)
		{
			temp = (int)ptr[bx];
			hist.flush(temp, 1);
		}
	}

	ptr = (uchar *) m_SmoothImage->imageData;
	temp = hist.GetValueSN(0);
	ptr[x] = (uchar) temp;
	
	while (x < Width)
	{
		switch( flag )
		{
		// Down
		case MOVE_DOWN:
			// Subtract the old line
			DealMove( hist, x, y - bias, Y_MOVE_SUB );
			y++;
			DealMove( hist, x, y + bias, Y_MOVE_ADD );
			break;
		// Up
		case MOVE_UP:
			// Subtract the old line
			DealMove( hist, x, y + bias, Y_MOVE_SUB );
			y--;
			DealMove( hist, x, y - bias, Y_MOVE_ADD );
			break;
		}
		ptr = (uchar *) (m_SmoothImage->imageData + y * m_SmoothImage->widthStep);
		temp = hist.GetValueSN(0);
		ptr[x] = (uchar) temp;

		if( (y == 0) || (y == Height - 1) )
		{
			DealMove( hist, x - bias, y, X_MOVE_SUB );
			x++;
			DealMove( hist, x + bias, y, X_MOVE_ADD );
			ptr = (uchar *) (m_SmoothImage->imageData + y * m_SmoothImage->widthStep);
			temp = hist.GetValueSN(0);
			ptr[x] = (uchar) temp;
			flag = 1 - flag;
		}
	}
}

void CorrosionDehaze::preDoEnhance(IplImage *resultbuf)
{
	common::BilateralFilter BFilter( m_Bilateral_Winsize,
		m_Bilateral_Dr, m_Bilateral_Dd);
	BFilter.doBilateralFilter(m_DarkestImage, resultbuf);
}

void CorrosionDehaze::doEnhance(IplImage *referImage)
{
	common::GuidedBilateralFilter SJFilter( m_Joint_Winsize, m_Joint_DrC, 
		m_Joint_DrL, m_Joint_DdL);
	SJFilter.doGuidedBilateralFilter( m_SmoothImage, referImage, m_BilaFilImage );
}

void CorrosionDehaze::DealMove(Histogram &hist, int x, int y, int flag)
{
	int Height = m_DarkestImage->height;
	int Width  = m_DarkestImage->width;
	int bias = (int) (WIN / 2);
	int currentRange;
	uchar *ptr;
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
				ptr = (uchar *) (m_DarkestImage->imageData + y * m_DarkestImage->widthStep);
				currentRange = (int) ptr[x + b];
				IsLegal = true;
			}
			break;
		case X_MOVE_ADD:
		case X_MOVE_SUB:
			if( (x >= 0) && (x < Width) && ((b + y) >= 0) && ((b + y) < Height) )
			{
				ptr = (uchar *) (m_DarkestImage->imageData + (y + b) * m_DarkestImage->widthStep);
				currentRange = (int) ptr[x];
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
				hist.flush(currentRange, 1);
				break;
			case Y_MOVE_SUB:
			case X_MOVE_SUB:
				hist.flush(currentRange, -1);
				break;
			}
		}
	}
}