#include "SoundDev.h"

#pragma once
#ifndef __FMS_H__
#define __FMS_H__

class CFMS : public CSoundDevice
{
	virtual void UpdateVolExp(UINT8 ch);
	virtual void UpdateVoice(UINT8 ch);
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum);
	virtual void UpdatePanpot(UINT8 ch);
	virtual void UpdateSustain(UINT8 ch);
	virtual void UpdateTL(UINT8 ch, UINT8 op, UINT8 lev);
	virtual void UpdateKey(UINT8 ch, UINT8 keyon);
public:
	CFMS(CPort* pt, int fsamp);
};

#endif
