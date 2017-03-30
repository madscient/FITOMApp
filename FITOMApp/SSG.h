#ifndef __SSG_H__
#define __SSG_H__

#include "SoundDev.h"

class CPSGBase : public CSoundDevice
{
protected:
	CEnvelope* egattr;
	UINT8* lfoTL;

	virtual FNUM GetFnumber(UINT8 ch, SINT16 offset = 0);

	virtual void UpdateVolExp(UINT8 ch) = 0;
	virtual void UpdateVoice(UINT8 ch) {};
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum) = 0;
	virtual void UpdatePanpot(UINT8 ch) {};
	virtual void UpdateRhythmVol() {};
	virtual void UpdateLevelEG();
	virtual void UpdateSustain(UINT8 ch) {};
	virtual void UpdateKey(UINT8 ch, UINT8 keyon);
	virtual void UpdateTL(UINT8 ch, UINT8 op, UINT8 lev) {};
public:
	CPSGBase(UINT8 devid, CPort* pt, UINT8 ch, int fsamp);
	virtual void RhythmOn(UINT8 num, UINT8 vel) {};
	virtual void PollingCallBack();
	virtual void TimerCallBack(UINT32 tick);
};

class CSSG : public CPSGBase
{
public:
	CSSG(CPort* pt, int fsamp);
	virtual UINT8 QueryCh(CMidiCh* parent, FMVOICE* voice, int mode);
protected:
	virtual void UpdateTL(UINT8 ch, UINT8 op, UINT8 lev);
	virtual void UpdateVolExp(UINT8 ch);
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum);
	virtual void UpdateVoice(UINT8 ch);
};

class CSSGS : public CPSGBase
{
public:
	CSSGS(CPort* pt, int fsamp);
	virtual UINT8 QueryCh(CMidiCh* parent, FMVOICE* voice, int mode);
protected:
	virtual void UpdateTL(UINT8 ch, UINT8 op, UINT8 lev);
	virtual void UpdateVolExp(UINT8 ch);
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum);
	virtual void UpdateVoice(UINT8 ch);
	virtual void UpdatePanpot(UINT8 ch);
};

class CDCSG : public CPSGBase
{
public:
	CDCSG(CPort* pt, int fsamp);
	virtual UINT8 QueryCh(CMidiCh* parent, FMVOICE* voice, int mode);
protected:
	virtual void UpdateVolExp(UINT8 ch);
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum);
	virtual void UpdateVoice(UINT8 ch);
	virtual UINT8 AllocCh(CMidiCh* parent, FMVOICE* voice = 0);
};

class CSAA : public CPSGBase
{
public:
	CSAA(CPort* pt, int fsamp);
	virtual UINT8 QueryCh(CMidiCh* parent, FMVOICE* voice, int mode);
protected:
	virtual FNUM GetFnumber(UINT8 ch);
	virtual void UpdateTL(UINT8 ch, UINT8 op, UINT8 lev);
	virtual void UpdateVolExp(UINT8 ch);
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum);
	virtual void UpdateVoice(UINT8 ch);
	virtual void UpdatePanpot(UINT8 ch);
};

#if 0
class CSCC : public CPSGBase
{
};


class CSSGS : public CPSGBase
{
};
#endif

class CAPSG : public CPSGBase
{
protected:
	virtual void UpdateVolExp(UINT8 ch);
	virtual void UpdateVoice(UINT8 ch);
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum);
	virtual void UpdateTL(UINT8 ch, UINT8 op, UINT8 lev);
public:
	CAPSG(CPort* pt, UINT8 fsamp = 0);
};

class CDSG : public CSoundDevice
{
public:
	CDSG(CPort* pt, int fsamp);
	UINT8 regbak[0x20];
protected:
	UINT8 RhythmOnMap;
	UINT8 RhythmOffMap;
	virtual void SetReg(UINT16 addr, UINT8 data, int v=1);
	virtual void UpdateTL(UINT8 ch, UINT8 op, UINT8 lev);
	virtual void UpdateVolExp(UINT8 ch);
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum);
	virtual void UpdateVoice(UINT8 ch);
	virtual void UpdatePanpot(UINT8 ch);
	virtual void UpdateKey(UINT8 ch, UINT8 keyon);
	virtual void UpdateSustain(UINT8 ch);
	virtual void RhythmOn(UINT8 num, UINT8 vel, SINT8 pan, FMVOICE* rv, FNUM* fnum);
	virtual void RhythmOff(UINT8 num);
};

#endif
