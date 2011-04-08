#include "HistMedianFilter.h"

// 默认构造函数
HistMedianFilter::HistMedianFilter()
{
	m_src     = NULL;
	m_result  = NULL;
	m_winsize = 0   ;
}

// 三个参数的函数
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

// 调用中值滤波主处理过程，并向外界返回结果
IplImage * HistMedianFilter::HMedianFilter()
{
	mainprocess();
	return m_result;
}

// 根据x，y计算指定窗格的中值的索引
int HistMedianFilter::CalMedian(int x, int y)
{
	int height = m_src->height;
	int width  = m_src->width;
	int bias   = m_winsize/2;
	int tempx,tempy,median;

	// 求x方向上的合法长度
	if(x - bias < 0)
		tempx = x + bias + 1;
	else if(x + bias >= width)
		tempx = width - x + bias;
	else
		tempx = m_winsize;

	// 求y方向上的合法长度
	if(y - bias < 0)
		tempy = y + bias + 1;
	else if(y + bias >= height)
		tempy = height - y + bias;
	else
		tempy = m_winsize;
	
	median = tempx * tempy / 2; 
	return median;
}

// 处理行的加减，flag : 1 : 加 -1 : 减 
void HistMedianFilter::DealRow(int col,int row,int flag)
{
	// flag只能是1或者-1
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

// 处理列的加减，flag : 1 : 加 -1 : 减
void HistMedianFilter::DealCol(int col,int row,int flag)
{
	// 限制flag的值
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

// 主要处理函数
void HistMedianFilter::mainprocess()
{
	// 初始化直方图和变量值
	int bias = m_winsize/2;
	int flag = 0;  // 方向
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
		// 向下
		case 0:
			// 减去旧行
			DealRow(x,y - bias,-1);
			// 向下移动
			y++;
			// 加上最下面的一行
			DealRow(x,y + bias,1);

			// 取中值
			rptr    = (uchar *)(m_result->imageData + y * m_result->widthStep);
			median  = CalMedian(x,y);
			rptr[x] = m_hist.GetValueSN(median);
			break;

		// 向上
		case 1:
			// 减去一行
			DealRow(x,y + bias,-1);
			// 向上移动
			y--;
			// 加上新行
			DealRow(x,y - bias,1);

			// 取中值
			rptr    = (uchar *)(m_result->imageData + y * m_result->widthStep);
			median  = CalMedian(x,y);
			rptr[x] = m_hist.GetValueSN(median);
			break;

		default:
			break;
		}

		// 到头了,向右移
		if(y == 0 || y == height -1)
		{
		    DealCol(x - bias,y,-1);
			x++; // 右移
			DealCol(x + bias,y,1);

			median  = CalMedian(x,y);
			rptr[x] = m_hist.GetValueSN(median);

			flag = 1 - flag;
		}
	}
}

