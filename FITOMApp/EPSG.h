#pragma once
#ifndef __EPSGH__
#define __EPSG_H__

#include "PSGBase.h"

class CEPSG : public CPSGBase
{
protected:
	uint8_t prevvol[3];
	uint16_t prevfreq[3];
	virtual void UpdateKey(uint8_t ch, uint8_t keyon);
	virtual void UpdateVolExp(uint8_t ch);
	virtual void UpdateVoice(uint8_t ch);
	virtual void UpdateFreq(uint8_t ch, const FNUM* fnum);
	virtual void UpdateTL(uint8_t ch, uint8_t op, uint8_t lev);
	virtual void EGOff(uint8_t ch);
	uint8_t prevmix;
public:
	CEPSG(CPort* pt, int fsamp = 0);
	virtual void Init();
};

#endif
