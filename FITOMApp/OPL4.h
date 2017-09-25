#ifndef __OPL4_H__
#define __OPL4_H__

#include "FITOMdefine.h"
#include "SoundDev.h"
#include "MIDI.h"

class COPL4ML : public CSoundDevice
{
protected:
	virtual ISoundDevice::FNUM GetFnumber(UINT8 ch, SINT16 offset);
	virtual void UpdateVolExp(UINT8 ch);
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum);
	virtual void UpdateKey(UINT8 ch, UINT8 keyon);
	virtual void UpdateVoice(UINT8 ch);
	virtual void UpdatePanpot(UINT8 ch);
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
