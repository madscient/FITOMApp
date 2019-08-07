#pragma once
#ifndef __SAA_H__
#define __SAA_H__

#include "PSGBase.h"

class CSAA : public CPSGBase
{
public:
	CSAA(CPort* pt, int fsamp);
	virtual void Init();
	//virtual uint8_t QueryCh(CMidiCh* parent, FMVOICE* voice, int mode);
	virtual void SetReg(uint16_t reg, uint8_t data, uint8_t v = 1);
protected:
	//virtual FNUM GetFnumber(uint8_t ch);
	virtual void UpdateTL(uint8_t ch, uint8_t op, uint8_t lev);
	virtual void UpdateVolExp(uint8_t ch);
	virtual void UpdateFreq(uint8_t ch, const FNUM* fnum);
	virtual void UpdateVoice(uint8_t ch);
	//virtual void UpdatePanpot(uint8_t ch);
};

#endif
