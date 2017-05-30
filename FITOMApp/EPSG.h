#pragma once
#ifndef __EPSGH__
#define __EPSG_H__

#include "PSGBase.h"

class CEPSG : public CPSGBase
{
protected:
	UINT8 prevvol[3];
	UINT16 prevfreq[3];
	virtual void UpdateKey(UINT8 ch, UINT8 keyon);
	virtual void UpdateVolExp(UINT8 ch);
	virtual void UpdateVoice(UINT8 ch);
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum);
	virtual void UpdateTL(UINT8 ch, UINT8 op, UINT8 lev);
	UINT8 prevmix;
public:
	CEPSG(CPort* pt, int fsamp = 0);
};

#endif
