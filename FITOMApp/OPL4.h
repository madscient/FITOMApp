#ifndef __OPL4_H__
#define __OPL4_H__

#include "FITOMdefine.h"
#include "SoundDev.h"
#include "MIDI.h"

class COPL4ML : public CSoundDevice
{
protected:
	virtual ISoundDevice::FNUM GetFnumber(uint8_t ch, int16_t offset);
	virtual void UpdateVolExp(uint8_t ch);
	virtual void UpdateFreq(uint8_t ch, const FNUM* fnum);
	virtual void UpdateKey(uint8_t ch, uint8_t keyon);
	virtual void UpdateVoice(uint8_t ch);
	virtual void UpdatePanpot(uint8_t ch);
public:
	COPL4ML(CPort* pt, int fsamp);
	virtual void Init();
};

class COPL4ML2 : public COPL4ML
{
public:
	COPL4ML2(CPort* pt, int fsamp);
};


#endif	//__OPL4_H__
