#pragma once
#ifndef __FMS_H__
#define __FMS_H__

#include "FITOMdefine.h"
#include "SoundDev.h"

class CFMS : public CSoundDevice
{
protected:
	uint8_t prevvol[16];
	uint8_t prevpan[16];
	uint16_t prevfnum[16];
	uint16_t prevkey;
	virtual void UpdateVolExp(uint8_t ch);
	virtual void UpdateVoice(uint8_t ch);
	virtual void UpdateFreq(uint8_t ch, const FNUM* fnum);
	virtual void UpdatePanpot(uint8_t ch);
	virtual void UpdateSustain(uint8_t ch);
	virtual void UpdateTL(uint8_t ch, uint8_t op, uint8_t lev);
	virtual void UpdateKey(uint8_t ch, uint8_t keyon);
public:
	CFMS(CPort* pt, int fsamp);
	virtual void Init();
};

#endif
