#pragma once
#ifndef __PCMD8_H__
#define __PCMD8_H__

#include "FITOMdefine.h"
#include "ADPCM.h"

class CAdPcmZ280 : public CAdPcmBase	//YAMAHA PCM8 ADPCM
{
protected:
	virtual ISoundDevice::FNUM GetFnumber(UINT8 ch, SINT16 offset);
	virtual void UpdateVolExp(UINT8 ch);
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum);
	virtual void UpdateKey(UINT8 ch, UINT8 keyon);
	virtual void UpdateVoice(UINT8 ch);
	virtual void UpdatePanpot(UINT8 ch);
	virtual UINT16 GetDeltaN(int off);
public:
	CAdPcmZ280(CPort* pt, int fsamp, size_t memsize);
	virtual void Init();
	virtual UINT8 QueryCh(CMidiCh* parent, FMVOICE* voice, int mode) { return 0; };
	virtual void LoadVoice(int prog, UINT8* data, size_t length);
};

#endif
