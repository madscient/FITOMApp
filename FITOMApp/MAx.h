#pragma once
#ifndef __MAx_H__
#define __MAx_H__

#include "FITOMdefine.h"
#include "SoundDev.h"

class CSD1 : public CSoundDevice {
protected:
	FMVOICE PresetTone[16];
	uint8_t Instrument[16];
	int PresetIndex;
	virtual void UpdateVolExp(uint8_t ch);
	virtual void UpdateVoice(uint8_t ch);
	virtual void UpdateFreq(uint8_t ch, const FNUM* fnum);
	virtual void UpdateTL(uint8_t ch, uint8_t op, uint8_t lev);
	virtual void UpdateKey(uint8_t ch, uint8_t keyon);
	virtual void UpdateSustain(uint8_t ch) {};
	virtual void UpdatePanpot(uint8_t ch) {};
	virtual void ResetChannel(uint8_t ch);
	void register_dump();
public:
	CSD1(CPort* pt, int fsamp);
	virtual void Init();
	virtual void SetVoice(uint8_t ch, FMVOICE* voice, int update = 1);
	virtual void UpdatePresetTone();
};

#endif __MAx_H__
