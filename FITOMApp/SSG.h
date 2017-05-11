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
	CPSGBase(UINT8 devid, CPort* pt, UINT8 ch, int fsamp, int div, int off, FnumTableType type);
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

class CPSG : public CSSG
{
public:
	CPSG(CPort* pt, int fsamp);
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
};

class CSAA : public CPSGBase
{
public:
	CSAA(CPort* pt, int fsamp);
	//virtual UINT8 QueryCh(CMidiCh* parent, FMVOICE* voice, int mode);
protected:
	//virtual FNUM GetFnumber(UINT8 ch);
	virtual void UpdateTL(UINT8 ch, UINT8 op, UINT8 lev);
	virtual void UpdateVolExp(UINT8 ch);
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum);
	virtual void UpdateVoice(UINT8 ch);
	//virtual void UpdatePanpot(UINT8 ch);
};

class CSCC : public CPSGBase
{
public:
	CSCC(CPort* pt, int fsamp);
protected:
	virtual void UpdateTL(UINT8 ch, UINT8 op, UINT8 lev);
	virtual void UpdateVolExp(UINT8 ch);
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum);
	virtual void UpdateVoice(UINT8 ch);
};

class CSCCP : public CPSGBase
{
public:
	CSCCP(CPort* pt, int fsamp);
protected:
	virtual void UpdateTL(UINT8 ch, UINT8 op, UINT8 lev);
	virtual void UpdateVolExp(UINT8 ch);
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum);
	virtual void UpdateVoice(UINT8 ch);
};

class CEPSG : public CPSGBase
{
protected:
	virtual void UpdateVolExp(UINT8 ch);
	virtual void UpdateVoice(UINT8 ch);
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum);
	virtual void UpdateTL(UINT8 ch, UINT8 op, UINT8 lev);
	UINT8 prevmix;
public:
	CEPSG(CPort* pt, int fsamp = 0);
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
