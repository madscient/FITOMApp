#ifndef __SSG_H__
#define __SSG_H__

#include "PSGBase.h"


class CSSG : public CPSGBase
{
public:
	CSSG(CPort* pt, int fsamp, uint8_t dev = DEVICE_SSG);
	virtual void Init();
	virtual uint8_t QueryCh(CMidiCh* parent, FMVOICE* voice, int mode);
protected:
	virtual void UpdateKey(uint8_t ch, uint8_t keyon);
	virtual void UpdateTL(uint8_t ch, uint8_t op, uint8_t lev);
	virtual void UpdateVolExp(uint8_t ch);
	virtual void UpdateFreq(uint8_t ch, const FNUM* fnum);
	virtual void UpdateVoice(uint8_t ch);
	virtual void EGOff(uint8_t ch);
};

class CPSG : public CSSG
{
public:
	CPSG(CPort* pt, int fsamp) : CSSG(pt, fsamp, DEVICE_PSG) {};
};

class CSSGLP : public CSSG
{
public:
	CSSGLP(CPort* pt, int fsamp) : CSSG(pt, fsamp, DEVICE_SSGLP) {};
};

class CSSGD : public CSSG
{
public:
	CSSGD(CPort* pt, int fsamp) : CSSG(pt, fsamp, DEVICE_SSGD) {};
};

#endif
