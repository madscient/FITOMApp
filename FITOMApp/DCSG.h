#pragma once
#ifndef __DCSG_H__
#define __DCSG_H__

#include "PSGBase.h"

class CDCSG : public CPSGBase
{
public:
	CDCSG(CPort* pt, int fsamp);
	virtual void Init();
	virtual uint8_t QueryCh(CMidiCh* parent, FMVOICE* voice, int mode);
protected:
	uint8_t prevnoise;
	uint8_t prevvol[4];
	uint16_t prevfreq[4];
	virtual void UpdateVolExp(uint8_t ch);
	virtual void UpdateFreq(uint8_t ch, const FNUM* fnum);
	virtual void UpdateVoice(uint8_t ch);
};

#endif
