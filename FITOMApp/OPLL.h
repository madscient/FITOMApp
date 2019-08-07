#ifndef __OPLL_H__
#define __OPLL_H__

#include "FITOMdefine.h"
#include "SoundDev.h"
#include "MIDI.h"

class COPLL : public CSoundDevice {
	friend class COPLLRhythm;
protected:
	virtual void UpdateVolExp(uint8_t ch);
	virtual void UpdateVoice(uint8_t ch);
	virtual void UpdateFreq(uint8_t ch, const FNUM* fnum);
	virtual void UpdatePanpot(uint8_t ch) {};
	virtual void UpdateSustain(uint8_t ch);
	virtual void UpdateKey(uint8_t ch, uint8_t keyon);
	virtual void UpdateTL(uint8_t ch, uint8_t op, uint8_t lev);
	virtual void ChangeRhythmMode(int mode);
public:
	COPLL(CPort* pt, uint8_t mode, int fsamp, uint8_t devtype=DEVICE_OPLL);
	virtual void Init();
	virtual void SetVoice(uint8_t ch, FMVOICE* voice, int update);
	virtual void TimerCallBack(uint32_t tick);
};

class COPLLP : public COPLL {
public:
	COPLLP(CPort* pt, uint8_t mode, int fsamp);
};

class COPLLX : public COPLL {
public:
	COPLLX(CPort* pt, uint8_t mode, int fsamp);
};

class COPLL2 : public COPLL {
protected:
	virtual void UpdateFreq(uint8_t ch, const FNUM* fnum);
public:
	COPLL2(CPort* pt, uint8_t mode, int fsamp);
};

class COPLLRhythm : public CRhythmDevice {
protected:
	uint8_t RhythmOnMap;
	uint8_t RhythmOffMap;
	static uint8_t RhythmFreq[3];
	static FNUM RhythmFnum[3];
	static uint8_t RhythmReg[];
	static uint8_t RhythmMapCh[];
	//Updater
	virtual void UpdateVolExp(uint8_t ch);
	virtual void UpdateFreq(uint8_t ch, const FNUM* fnum = 0);
	virtual void UpdateVoice(uint8_t ch) {};
	virtual void UpdatePanpot(uint8_t ch) {};
	virtual void UpdateKey(uint8_t ch, uint8_t keyon);
public:
	virtual void Init();
	virtual uint8_t QueryCh(CMidiCh* parent, FMVOICE* voice, int mode);
	COPLLRhythm(COPLL* parent);
};

#endif
