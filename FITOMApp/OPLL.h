#ifndef __OPLL_H__
#define __OPLL_H__

#include "FITOMdefine.h"
#include "SoundDev.h"
#include "MIDI.h"

class COPLL : public CSoundDevice {
protected:
	virtual void UpdateVolExp(UINT8 ch);
	virtual void UpdateVoice(UINT8 ch);
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum);
	virtual void UpdatePanpot(UINT8 ch) {};
	virtual void UpdateSustain(UINT8 ch);
	virtual void UpdateKey(UINT8 ch, UINT8 keyon);
	virtual void UpdateTL(UINT8 ch, UINT8 op, UINT8 lev);
	UINT8 RhythmOnMap;
	UINT8 RhythmOffMap;
	static UINT8 RhythmFreq[3];
	static FNUM RhythmFnum[3];
	static UINT8 RhythmReg[];
	static UINT8 RhythmMapCh[];
public:
	COPLL(CPort* pt, UINT8 mode, int fsamp, UINT8 devtype=DEVICE_OPLL);
	virtual void SetVoice(UINT8 ch, FMVOICE* voice, int update);
	virtual void RhythmOn(UINT8 num, UINT8 vel, SINT8 pan, FMVOICE* rv, FNUM* fnum);
	virtual void RhythmOff(UINT8 num);
	virtual void TimerCallBack(UINT32 tick);
};

class COPLLP : public COPLL {
public:
	COPLLP(CPort* pt, UINT8 mode, int fsamp);
};

class COPLLX : public COPLL {
public:
	COPLLX(CPort* pt, UINT8 mode, int fsamp);
};

class COPLL2 : public COPLL {
protected:
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum);
public:
	COPLL2(CPort* pt, UINT8 mode, int fsamp);
};

#endif
