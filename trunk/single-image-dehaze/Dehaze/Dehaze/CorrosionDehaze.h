#ifndef CORROSION_DEHAZE_H
#define CORROSION_DEHAZE_H

#define Y_MOVE_ADD 1
#define Y_MOVE_SUB -1
#define X_MOVE_ADD 2
#define X_MOVE_SUB -2

#define MOVE_DOWN 0
#define MOVE_UP	  1

#include "DehazeBase.h"

class Histogram;

class CorrosionDehaze : public DehazeBase
{
	void doSmooth();
	void preDoEnhance(IplImage *resultbuf);
	void doEnhance(IplImage *referImage);
	void DealMove(Histogram &hist, int x, int y, int flag);
};

#endif