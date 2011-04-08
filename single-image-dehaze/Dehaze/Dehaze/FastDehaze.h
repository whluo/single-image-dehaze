#ifndef FAST_BASE_H
#define FAST_BASE_H

#include "DehazeBase.h"

class FastDehaze : public DehazeBase
{
	void preDoEnhance(IplImage *resultbuf);
	void doEnhance(IplImage *referImage);
};

#endif