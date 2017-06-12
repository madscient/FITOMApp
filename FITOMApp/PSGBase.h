#pragma once
#ifndef __PSGBASE_H__
#define __PSGBASE_H__

#include "SoundDev.h"

class CPSGBase : public CSoundDevice
{
protected:
	CEnvelope* egattr;
	UINT8* lfoTL;

	virtual FNUM GetFnumber(UINT8 ch, SINT16 offset = 0);

	virtual void UpdateVolExp(UINT8 ch) = 0;
	virtual void UpdateVoice(UINT8 ch) {};
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum) = 0;
	virtual void UpdatePanpot(UINT8 ch) {};
	virtual void UpdateRhythmVol() {};
	virtual void UpdateLevelEG();
	virtual void UpdateSustain(UINT8 ch) {};
	virtual void UpdateKey(UINT8 ch, UINT8 keyon);
	virtual void UpdateTL(UINT8 ch, UINT8 op, UINT8 lev) {};
	virtual void EGOff(UINT8 ch) { NoteOff(ch); };
public:
	CPSGBase(UINT8 devid, CPort* pt, int regsize, UINT8 ch, int fsamp);
	CPSGBase(UINT8 devid, CPort* pt, int regsize, UINT8 ch, int fsamp, int div, int off, FnumTableType type);
	virtual void RhythmOn(UINT8 num, UINT8 vel) {};
	virtual void PollingCallBack();
	virtual void TimerCallBack(UINT32 tick);
};

#endif
