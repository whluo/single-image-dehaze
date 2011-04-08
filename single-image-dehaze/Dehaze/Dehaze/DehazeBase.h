#ifndef DEHAZE_BASE_H
#define DEHAZE_BASE_H

#include "cv.h"
#include "highgui.h"

class DehazeBase
{
public:
	DehazeBase();

	void setMedianParam(unsigned int Median1_Winsize, unsigned int Median2_Winsize);
	void setBilateralParam(unsigned int Bilateral_Winsize, double Bilateral_Dr,
		double Bilateral_Dd, unsigned int Bilateral_BinNum);
	void setJointParam(unsigned int Joint_WinSize, double Joint_DrL, double Joint_DrC,
		double Joint_DdL, unsigned int Joint_BinNum);

	void ReleaseAll();

	IplImage *getDarkestImage() const { return m_DarkestImage; };
	IplImage *getTranMapImage() const { return m_TranMapImage; };
	IplImage *getSmoothImage()  const { return m_SmoothImage;  };
	IplImage *getBilaFilImage() const { return m_BilaFilImage; };

	IplImage *doDehaze(const IplImage *src);

	~DehazeBase();

protected:
	virtual void preDoEnhance(IplImage *resultbuf) = 0;
	virtual void doEnhance(IplImage *referImage) = 0;
	virtual void doSmooth();

	int  generateAirligth();
	void generateTranMap(int airlight, double *TranMap);
	void generateDarkestImage(const IplImage *src);
	void generateTranImage(double *TranMap);
	void doDehaze(const IplImage *src, double *TranMap, int airlight);

	void doImageSubtract(const IplImage *src1, const IplImage *src2, IplImage *result);

protected:
	// Parameters
	unsigned int m_Median1_Winsize;
	unsigned int m_Median2_Winsize;
	unsigned int m_Bilateral_Winsize;
	unsigned int m_Joint_Winsize;

	double m_Bilateral_Dr;
	double m_Bilateral_Dd;
	double m_Joint_DrL;
	double m_Joint_DrC;
	double m_Joint_DdL;

	unsigned int m_Bilateral_BinNum;
	unsigned int m_Joint_BinNum;

	// Median result
	IplImage *m_DarkestImage;
	IplImage *m_TranMapImage;
	IplImage *m_SmoothImage;
	IplImage *m_BilaFilImage;

	// result
	IplImage *m_Result;
};


#endif
