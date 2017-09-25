#pragma once
#ifndef __DCSG_H__
#define __DCSG_H__

#include "PSGBase.h"

class CDCSG : public CPSGBase
{
public:
	CDCSG(CPort* pt, int fsamp);
	virtual void Init();
	virtual UINT8 QueryCh(CMidiCh* parent, FMVOICE* voice, int mode);
protected:
	UINT8 prevnoise;
	UINT8 prevvol[4];
	UINT16 prevfreq[4];
	virtual void UpdateVolExp(UINT8 ch);
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum);
	virtual void UpdateVoice(UINT8 ch);
};

#endif
