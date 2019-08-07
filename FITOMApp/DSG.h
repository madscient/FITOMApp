#pragma once
#ifndef __DSG_H__
#define __DSG_H__

#include "FITOMdefine.h"
#include "SoundDev.h"

class CDSG : public CSoundDevice
{
public:
	CDSG(CPort* pt, int fsamp);
	virtual void Init();
protected:
	uint8_t regbak[0x20];
//	uint8_t RhythmOnMap;
//	uint8_t RhythmOffMap;
	virtual void SetReg(uint16_t addr, uint8_t data, uint8_t v = 1);
	virtual void UpdateTL(uint8_t ch, uint8_t op, uint8_t lev);
	virtual void UpdateVolExp(uint8_t ch);
	virtual void UpdateFreq(uint8_t ch, const FNUM* fnum);
	virtual void UpdateVoice(uint8_t ch);
	virtual void UpdatePanpot(uint8_t ch);
	virtual void UpdateKey(uint8_t ch, uint8_t keyon);
	virtual void UpdateSustain(uint8_t ch);
//	virtual void RhythmOn(uint8_t num, uint8_t vel, int8_t pan, FMVOICE* rv, FNUM* fnum);
//	virtual void RhythmOff(uint8_t num);
};

#endif

