#include "CommonDehaze.h"
#include "DehazeTools.h"
#include "Set.h"

void CommonDehaze::preDoEnhance(IplImage *resultbuf)
{
	common::BilateralFilter BFilter( m_Bilateral_Winsize,
		m_Bilateral_Dr, m_Bilateral_Dd);
	BFilter.doBilateralFilter(m_DarkestImage, resultbuf);
}

void CommonDehaze::doEnhance(IplImage *referImage)
{
	common::GuidedBilateralFilter SJFilter( m_Joint_Winsize, m_Joint_DrC, 
		m_Joint_DrL, m_Joint_DdL);
	SJFilter.doGuidedBilateralFilter( m_SmoothImage, referImage, m_BilaFilImage );
}
