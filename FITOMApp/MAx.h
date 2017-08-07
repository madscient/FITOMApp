#pragma once
#ifndef __MAx_H__
#define __MAx_H__

#include "SoundDev.h"

class CSD1 : public CSoundDevice {
protected:
	FMVOICE PresetTone[16];
	virtual void UpdateVolExp(UINT8 ch);
	virtual void UpdateVoice(UINT8 ch);
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum);
	virtual void UpdatePanpot(UINT8 ch);
	virtual void UpdateSustain(UINT8 ch);
	virtual void UpdateTL(UINT8 ch, UINT8 op, UINT8 lev);
	virtual void UpdateKey(UINT8 ch, UINT8 keyon);
public:
	CSD1(CPort* pt, int fsamp);
	virtual void SetReg(UINT16 reg, UINT8 data, UINT8 v = 1);
	virtual void Flush();
	virtual void SetPresetTone(UINT8 tn, FMVOICE* voice);
	virtual void UpdatePresetTone();
};

#endif __MAx_H__
