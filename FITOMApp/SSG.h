#ifndef __SSG_H__
#define __SSG_H__

#include "PSGBase.h"


class CSSG : public CPSGBase
{
public:
	CSSG(CPort* pt, int fsamp, UINT8 dev = DEVICE_SSG);
	virtual void Init();
	virtual UINT8 QueryCh(CMidiCh* parent, FMVOICE* voice, int mode);
protected:
	virtual void UpdateKey(UINT8 ch, UINT8 keyon);
	virtual void UpdateTL(UINT8 ch, UINT8 op, UINT8 lev);
	virtual void UpdateVolExp(UINT8 ch);
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum);
	virtual void UpdateVoice(UINT8 ch);
	virtual void EGOff(UINT8 ch);
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
