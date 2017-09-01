#ifndef __OPL_H__
#define __OPL_H__

#include "FITOMdefine.h"
#include "MIDI.h"
#include "MultiDev.h"

class COPL : public CSoundDevice {
	friend class COPL3;
protected:
	virtual void UpdateVolExp(UINT8 ch);
	virtual void UpdateVoice(UINT8 ch);
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum);
	virtual void UpdatePanpot(UINT8 ch);
	virtual void UpdateSustain(UINT8 ch);
	virtual void UpdateTL(UINT8 ch, UINT8 op, UINT8 lev);
	virtual void UpdateKey(UINT8 ch, UINT8 keyon);
	UINT8 RhythmOnMap;
	UINT8 RhythmOffMap;
	static UINT8 RhythmFreq[];
	static FNUM RhythmFnum[];
	static UINT8 map[];
	static UINT8 RhythmReg[];
	static UINT8 RhythmMapCh[];
public:
	COPL(CPort* pt, int fsamp, UINT8 devtype=DEVICE_OPL);
	virtual void RhythmOn(UINT8 num, UINT8 vel, SINT8 pan, FMVOICE* rv, FNUM* fnum);
	virtual void RhythmOff(UINT8 num);
	virtual void TimerCallBack(UINT32 tick);
};

class C3801 : public COPL {
public:
	C3801(CPort* pt, int fsamp);
};

class COPL2 : public COPL {
public:
	COPL2(CPort* pt, int fsamp);
};

class COPL3 : public CSoundDevice {
protected:
	virtual void UpdateVolExp(UINT8 ch);
	virtual void UpdateVoice(UINT8 ch);
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum=0);
	virtual void UpdatePanpot(UINT8 ch);
	virtual void UpdateSustain(UINT8 ch);
	virtual void UpdateKey(UINT8 ch, UINT8 keyon);
	virtual void UpdateTL(UINT8 ch, UINT8 op, UINT8 lev);
	virtual void UpdateFnumber(UINT8 ch, int update=1);
	FNUM PseudoDT1[6];
	FNUM PseudoDT2[6];
	static UINT8 carmsk[];
	static UINT8 opmap[];
public:
	COPL3(CPort* pt, int fsamp);
};

class COPL3_2 : public CSpanDevice {
public:
	COPL3_2(CPort* pt1, CPort* pt2, UINT8 mode, int fsamp);
};

#endif
