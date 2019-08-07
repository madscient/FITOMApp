#ifndef __OPN_H__
#define __OPN_H__

#include "FITOMdefine.h"
#include "MultiDev.h"
#include "MIDI.h"

class COPN : public CSoundDevice {
	friend class COPN2;
protected:
	virtual void UpdateVolExp(uint8_t ch);
	virtual void UpdateVoice(uint8_t ch);
	virtual void UpdateFreq(uint8_t ch, const FNUM* fnum);
	virtual void UpdatePanpot(uint8_t ch);
	virtual void UpdateSustain(uint8_t ch);
	virtual void UpdateKey(uint8_t ch, uint8_t keyon);
	virtual void UpdateTL(uint8_t ch, uint8_t op, uint8_t lev);
	virtual void UpdateFnumber(uint8_t ch, int update);
	virtual uint8_t QueryCh(CMidiCh* parent, FMVOICE* voice, int mode);
	bool fxena;
	FNUM PseudoDT[4];
	static uint8_t map[4];
	static uint8_t carmsk[8];
public:
	COPN(CPort* pt, int fsamp, uint8_t devtype=DEVICE_OPN);
	virtual void Init();
};

class COPN2 : public CSpanDevice {
protected:
	virtual void UpdateVoice(uint8_t ch);
	virtual void UpdateKey(uint8_t ch, uint8_t keyon);
	LFORESOURCE* lfores;
public:
	COPN2(CPort* pt1, CPort* pt2, int fsamp, uint8_t devtype=DEVICE_OPN2);
	~COPN2();
	virtual void Init();
	virtual void EnableDevPM(uint8_t ch, uint8_t on);
	virtual void EnableDevAM(uint8_t ch, uint8_t on);
	virtual void SetDevPMDepth(uint8_t ch, uint8_t dep);
	virtual void SetDevAMDepth(uint8_t ch, uint8_t dep);
	virtual void SetDevPMRate(uint8_t ch, uint8_t rate);
	virtual void SetDevAMRate(uint8_t ch, uint8_t rate);
};

class COPNB : public COPN2 {
public:
	COPNB(CPort* pt1, CPort* pt2, int fsamp, uint8_t devtype=DEVICE_OPNB);
	virtual void Init();
};

class COPNA : public COPN2 {
protected:
public:
	COPNA(CPort* pt1, CPort* pt2, int fsamp, uint8_t devtype=DEVICE_OPNA);
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
	virtual void UpdateVolExp(uint8_t ch);
	virtual void UpdateFreq(uint8_t ch, const FNUM* fnum = 0) {};
	virtual void UpdateVoice(uint8_t ch) {};
	virtual void UpdatePanpot(uint8_t ch);
	virtual void UpdateKey(uint8_t ch, uint8_t keyon);
public:
	virtual void Init() {};
	virtual uint8_t QueryCh(CMidiCh* parent, FMVOICE* voice, int mode);
	COPNARhythm(COPNA* parent);
};

class COPN3LRhythm : public COPNARhythm {
public:
	COPN3LRhythm(COPN3L* parent);
};

#endif

