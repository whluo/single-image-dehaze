#include "Histogram.h"

Histogram::Histogram()
{
	int i;
	for(i=0;i<256;i++)
		m_h[i] = 0;
}

Histogram::Histogram(const Histogram &hist)
{
	int i;
	for(i=0;i<256;i++)
		m_h[i] = hist.m_h[i];
}

Histogram & Histogram::operator = (Histogram &hist)
{
	int i;
	for(i=0;i<256;i++)
		m_h[i] = hist.m_h[i];

	return (*this);
}

Histogram  Histogram::operator + (const Histogram &hist)
{
	int i;
	Histogram ahist;
	for(i=0;i<256;i++)
		ahist.m_h[i] = hist.m_h[i] + m_h[i];
	return ahist;
}

Histogram  Histogram::operator - (const Histogram &hist)
{
	int i;
	Histogram ahist;
	for(i=0;i<256;i++)
		ahist.m_h[i] = m_h[i] - hist.m_h[i];
	return ahist;
}

void Histogram::setzero()
{
	int i;
	for(i=0;i<256;i++)
		m_h[i] = 0;	
}

int Histogram::GetValueSN(int n)
{
	int sum = m_h[0];
	int i=0;
	while(sum<=n)
	{
		i++;
		sum += m_h[i];	
	}
	return i;
}

void Histogram::flush(int i , int flag)
{
	switch(flag)
	{
	case 1:
		m_h[i]++;
		break;
	case -1:
		m_h[i]--;
		break;
	default:
		break;
	}
}

void Histogram::flushn(int i,int n ,int flag)
{
	switch(flag)
	{
	case 1:
		m_h[i] += n;
		break;
	case -1:
		m_h[i] -= n;
		break;
	default:
		break;
	}
}

int Histogram::getNth(int n) const
{
	return m_h[n];
}