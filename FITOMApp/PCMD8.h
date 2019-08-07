#pragma once
#ifndef __PCMD8_H__
#define __PCMD8_H__

#include "FITOMdefine.h"
#include "ADPCM.h"

class CAdPcmZ280 : public CAdPcmBase	//YAMAHA PCM8 ADPCM
{
protected:
	virtual ISoundDevice::FNUM GetFnumber(uint8_t ch, int16_t offset);
	virtual void UpdateVolExp(uint8_t ch);
	virtual void UpdateFreq(uint8_t ch, const FNUM* fnum);
	virtual void UpdateKey(uint8_t ch, uint8_t keyon);
	virtual void UpdateVoice(uint8_t ch);
	virtual void UpdatePanpot(uint8_t ch);
	virtual uint16_t GetDeltaN(int off);
public:
	CAdPcmZ280(CPort* pt, int fsamp, size_t memsize);
	virtual void Init();
	virtual uint8_t QueryCh(CMidiCh* parent, FMVOICE* voice, int mode) { return 0; };
	virtual void LoadVoice(int prog, uint8_t* data, size_t length);
};

#endif
