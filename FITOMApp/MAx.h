#pragma once
#ifndef __MAx_H__
#define __MAx_H__

#include "SoundDev.h"

class CSD1 : public CSoundDevice {
protected:
	FMVOICE PresetTone[16];
	UINT8 Instrument[16];
	int PresetIndex;
	virtual void UpdateVolExp(UINT8 ch);
	virtual void UpdateVoice(UINT8 ch);
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum);
	virtual void UpdateTL(UINT8 ch, UINT8 op, UINT8 lev);
	virtual void UpdateKey(UINT8 ch, UINT8 keyon);
public:
	CSD1(CPort* pt, int fsamp);
	virtual void SetVoice(UINT8 ch, FMVOICE* voice, int update = 1);
	virtual void UpdatePresetTone();
};

#endif __MAx_H__
