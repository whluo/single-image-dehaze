#ifndef COMMON_DEHAZE_H
#define COMMON_DEHAZE_H

#include "DehazeBase.h"

class CommonDehaze : public DehazeBase
{
	void preDoEnhance(IplImage *resultbuf);
	void doEnhance(IplImage *referImage);
};

#endif