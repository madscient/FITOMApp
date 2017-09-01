#pragma once
#ifndef __DSG_H__
#define __DSG_H__

#include "FITOMdefine.h"
#include "SoundDev.h"

class CDSG : public CSoundDevice
{
public:
	CDSG(CPort* pt, int fsamp);
	UINT8 regbak[0x20];
protected:
	UINT8 RhythmOnMap;
	UINT8 RhythmOffMap;
	virtual void SetReg(UINT16 addr, UINT8 data, UINT8 v = 1);
	virtual void UpdateTL(UINT8 ch, UINT8 op, UINT8 lev);
	virtual void UpdateVolExp(UINT8 ch);
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum);
	virtual void UpdateVoice(UINT8 ch);
	virtual void UpdatePanpot(UINT8 ch);
	virtual void UpdateKey(UINT8 ch, UINT8 keyon);
	virtual void UpdateSustain(UINT8 ch);
	virtual void RhythmOn(UINT8 num, UINT8 vel, SINT8 pan, FMVOICE* rv, FNUM* fnum);
	virtual void RhythmOff(UINT8 num);
};

#endif

