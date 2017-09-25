#pragma once
#ifndef __SAA_H__
#define __SAA_H__

#include "PSGBase.h"

class CSAA : public CPSGBase
{
public:
	CSAA(CPort* pt, int fsamp);
	virtual void Init();
	//virtual UINT8 QueryCh(CMidiCh* parent, FMVOICE* voice, int mode);
	virtual void SetReg(UINT16 reg, UINT8 data, UINT8 v = 1);
protected:
	//virtual FNUM GetFnumber(UINT8 ch);
	virtual void UpdateTL(UINT8 ch, UINT8 op, UINT8 lev);
	virtual void UpdateVolExp(UINT8 ch);
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum);
	virtual void UpdateVoice(UINT8 ch);
	//virtual void UpdatePanpot(UINT8 ch);
};

#endif
