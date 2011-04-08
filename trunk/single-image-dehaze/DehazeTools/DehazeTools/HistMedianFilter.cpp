#include "HistMedianFilter.h"

// Ĭ�Ϲ��캯��
HistMedianFilter::HistMedianFilter()
{
	m_src     = NULL;
	m_result  = NULL;
	m_winsize = 0   ;
}

// ���������ĺ���
HistMedianFilter::HistMedianFilter(IplImage *src,int winsize, IplImage *result)
{
	m_src     =	src    ;
	m_winsize = winsize;
	m_result  = result ;
}

void HistMedianFilter::setimage(IplImage *src)     
{
	m_src     = src;    
}

void HistMedianFilter::sethist(Histogram hist)     
{
	m_hist    = hist;   
}

void HistMedianFilter::setwinsize(int winsize)     
{
	m_winsize = winsize;
}

void HistMedianFilter::setresult(IplImage *result) 
{
	m_result  = result; 
}

// ������ֵ�˲���������̣�������緵�ؽ��
IplImage * HistMedianFilter::HMedianFilter()
{
	mainprocess();
	return m_result;
}

// ����x��y����ָ���������ֵ������
int HistMedianFilter::CalMedian(int x, int y)
{
	int height = m_src->height;
	int width  = m_src->width;
	int bias   = m_winsize/2;
	int tempx,tempy,median;

	// ��x�����ϵĺϷ�����
	if(x - bias < 0)
		tempx = x + bias + 1;
	else if(x + bias >= width)
		tempx = width - x + bias;
	else
		tempx = m_winsize;

	// ��y�����ϵĺϷ�����
	if(y - bias < 0)
		tempy = y + bias + 1;
	else if(y + bias >= height)
		tempy = height - y + bias;
	else
		tempy = m_winsize;
	
	median = tempx * tempy / 2; 
	return median;
}

// �����еļӼ���flag : 1 : �� -1 : �� 
void HistMedianFilter::DealRow(int col,int row,int flag)
{
	// flagֻ����1����-1
	if((flag!=1) && (flag!=-1))
		return ;

	int height = m_src->height;
	int width  = m_src->width ;
	int bias   = m_winsize/2  ;

	int i;
	if((row < 0) || (row >= height))
		return;
	else
	{
		uchar * ptr = (uchar *)(m_src->imageData + row * m_src->widthStep);
		for(i=-bias;i<=bias;i++)
		{
			if( (col + i >= 0) && (col + i) < width)
				m_hist.flush((int)ptr[col+i],flag);
			else
				continue;
		}
	}
	return ;
}

// �����еļӼ���flag : 1 : �� -1 : ��
void HistMedianFilter::DealCol(int col,int row,int flag)
{
	// ����flag��ֵ
	if((flag!=1) && (flag!=-1))
		return ;

	int height = m_src->height;
	int width  = m_src->width ;
	int bias   = m_winsize/2  ;

	int i;
	if((col < 0) || (col >= width))
		return ;
	else
	{
		for(i=-bias;i<=bias;i++)
		{
			if((row + i >= 0) && (row + i) < height)
			{
				uchar * ptr = (uchar *)(m_src->imageData + (row + i) * m_src->widthStep);
				m_hist.flush((int)ptr[col],flag);
			}
			else
				continue;
		}
	}
	return ;
}

// ��Ҫ������
void HistMedianFilter::mainprocess()
{
	// ��ʼ��ֱ��ͼ�ͱ���ֵ
	int bias = m_winsize/2;
	int flag = 0;  // ����
	int x,y;
	int height = m_src->height;
	int width  = m_src->width;
	int median;
	m_hist.setzero();
	x = y = 0;

	int bx,by,temp;
	for(by=0;by<=bias;by++)
	{
		uchar * ptr = (uchar *)(m_src->imageData + (y + by) * m_src->widthStep);
		for(bx=0;bx<=bias;bx++)
		{
			temp = (int)ptr[x + bx];
			m_hist.flush(temp,1);
		}
	}

	uchar * rptr = (uchar *)(m_result->imageData + y * m_result->widthStep);
	median       = CalMedian(x,y);
	rptr[x]      = m_hist.GetValueSN(median);

	while(x < width)
	{
		switch(flag)
		{
		// ����
		case 0:
			// ��ȥ����
			DealRow(x,y - bias,-1);
			// �����ƶ�
			y++;
			// �����������һ��
			DealRow(x,y + bias,1);

			// ȡ��ֵ
			rptr    = (uchar *)(m_result->imageData + y * m_result->widthStep);
			median  = CalMedian(x,y);
			rptr[x] = m_hist.GetValueSN(median);
			break;

		// ����
		case 1:
			// ��ȥһ��
			DealRow(x,y + bias,-1);
			// �����ƶ�
			y--;
			// ��������
			DealRow(x,y - bias,1);

			// ȡ��ֵ
			rptr    = (uchar *)(m_result->imageData + y * m_result->widthStep);
			median  = CalMedian(x,y);
			rptr[x] = m_hist.GetValueSN(median);
			break;

		default:
			break;
		}

		// ��ͷ��,������
		if(y == 0 || y == height -1)
		{
		    DealCol(x - bias,y,-1);
			x++; // ����
			DealCol(x + bias,y,1);

			median  = CalMedian(x,y);
			rptr[x] = m_hist.GetValueSN(median);

			flag = 1 - flag;
		}
	}
}

