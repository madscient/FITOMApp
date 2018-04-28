#ifndef __OPN_H__
#define __OPN_H__

#include "FITOMdefine.h"
#include "MultiDev.h"
#include "MIDI.h"

class COPN : public CSoundDevice {
	friend class COPN2;
protected:
	virtual void UpdateVolExp(UINT8 ch);
	virtual void UpdateVoice(UINT8 ch);
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum);
	virtual void UpdatePanpot(UINT8 ch);
	virtual void UpdateSustain(UINT8 ch);
	virtual void UpdateKey(UINT8 ch, UINT8 keyon);
	virtual void UpdateTL(UINT8 ch, UINT8 op, UINT8 lev);
	virtual void UpdateFnumber(UINT8 ch, int update);
	virtual UINT8 QueryCh(CMidiCh* parent, FMVOICE* voice, int mode);
	bool fxena;
	FNUM PseudoDT[4];
	static UINT8 map[4];
	static UINT8 carmsk[8];
public:
	COPN(CPort* pt, int fsamp, UINT8 devtype=DEVICE_OPN);
	virtual void Init();
};

class COPN2 : public CSpanDevice {
protected:
	virtual void UpdateVoice(UINT8 ch);
	virtual void UpdateKey(UINT8 ch, UINT8 keyon);
	LFORESOURCE* lfores;
public:
	COPN2(CPort* pt1, CPort* pt2, int fsamp, UINT8 devtype=DEVICE_OPN2);
	~COPN2();
	virtual void Init();
	virtual void EnableDevPM(UINT8 ch, UINT8 on);
	virtual void EnableDevAM(UINT8 ch, UINT8 on);
	virtual void SetDevPMDepth(UINT8 ch, UINT8 dep);
	virtual void SetDevAMDepth(UINT8 ch, UINT8 dep);
	virtual void SetDevPMRate(UINT8 ch, UINT8 rate);
	virtual void SetDevAMRate(UINT8 ch, UINT8 rate);
};

class COPNB : public COPN2 {
public:
	COPNB(CPort* pt1, CPort* pt2, int fsamp, UINT8 devtype=DEVICE_OPNB);
	virtual void Init();
};

class COPNA : public COPN2 {
protected:
public:
	COPNA(CPort* pt1, CPort* pt2, int fsamp, UINT8 devtype=DEVICE_OPNA);
	virtual void Init();
};

class COPN3L : public COPNA {
protected:
public:
	COPN3L(CPort* pt1, CPort* pt2, int fsamp) : COPNA(pt1, pt2, fsamp, DEVICE_OPN3L) {};
	virtual void Init();
};

class COPNARhythm : public CRhythmDevice {
protected:
	//Updater
	virtual void UpdateVolExp(UINT8 ch);
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum = 0) {};
	virtual void UpdateVoice(UINT8 ch) {};
	virtual void UpdatePanpot(UINT8 ch);
	virtual void UpdateKey(UINT8 ch, UINT8 keyon);
public:
	virtual void Init() {};
	virtual UINT8 QueryCh(CMidiCh* parent, FMVOICE* voice, int mode);
	COPNARhythm(CSoundDevice* parent);
};

class COPN3LRhythm : public COPNARhythm {
public:
	COPN3LRhythm(CSoundDevice* parent) : COPN3LRhythm(parent) {};
};

#endif

