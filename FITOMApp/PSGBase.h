#pragma once
#ifndef __PSGBASE_H__
#define __PSGBASE_H__

#include "FITOMdefine.h"
#include "SoundDev.h"

class CPSGBase : public CSoundDevice
{
protected:
	CEnvelope* egattr;
	uint8_t* lfoTL;

	virtual FNUM GetFnumber(uint8_t ch, int16_t offset = 0);
	virtual void UpdateVolExp(uint8_t ch) = 0;
	virtual void UpdateVoice(uint8_t ch) {};
	virtual void UpdateFreq(uint8_t ch, const FNUM* fnum) = 0;
	virtual void UpdatePanpot(uint8_t ch) {};
	virtual void UpdateRhythmVol() {};
	virtual void UpdateLevelEG();
	virtual void UpdateSustain(uint8_t ch) {};
	virtual void UpdateKey(uint8_t ch, uint8_t keyon);
	virtual void UpdateTL(uint8_t ch, uint8_t op, uint8_t lev) {};
	virtual void EGOff(uint8_t ch) { NoteOff(ch); };
public:
	CPSGBase(uint8_t devid, CPort* pt, int regsize, uint8_t ch, int fsamp);
	CPSGBase(uint8_t devid, CPort* pt, int regsize, uint8_t ch, int fsamp, int div, int off, FnumTableType type);
	virtual void Init() {};
	virtual void RhythmOn(uint8_t num, uint8_t vel) {};
	virtual void PollingCallBack();
	virtual void TimerCallBack(uint32_t tick);
};

namespace ROM {
	extern const uint8_t VolCurveLin[];
};


#endif
