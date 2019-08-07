#ifndef __OPL_H__
#define __OPL_H__

#include "FITOMdefine.h"
#include "MIDI.h"
#include "MultiDev.h"

class COPL : public CSoundDevice {
	friend class COPL3;
protected:
	virtual void Init();
	virtual void UpdateVolExp(uint8_t ch);
	virtual void UpdateVoice(uint8_t ch);
	virtual void UpdateFreq(uint8_t ch, const FNUM* fnum);
	virtual void UpdatePanpot(uint8_t ch);
	virtual void UpdateSustain(uint8_t ch);
	virtual void UpdateTL(uint8_t ch, uint8_t op, uint8_t lev);
	virtual void UpdateKey(uint8_t ch, uint8_t keyon);
	static uint8_t map[];
public:
	COPL(CPort* pt, int fsamp, uint8_t devtype=DEVICE_OPL);
	virtual void TimerCallBack(uint32_t tick);
};

class C3801 : public COPL {
protected:
public:
	C3801(CPort* pt, int fsamp);
	virtual void Init() {};
};

class COPL2 : public COPL {
public:
	COPL2(CPort* pt, int fsamp);
};

class COPL3 : public CSoundDevice {
protected:
	virtual void UpdateVolExp(uint8_t ch);
	virtual void UpdateVoice(uint8_t ch);
	virtual void UpdateFreq(uint8_t ch, const FNUM* fnum=0);
	virtual void UpdatePanpot(uint8_t ch);
	virtual void UpdateSustain(uint8_t ch);
	virtual void UpdateKey(uint8_t ch, uint8_t keyon);
	virtual void UpdateTL(uint8_t ch, uint8_t op, uint8_t lev);
	virtual void UpdateFnumber(uint8_t ch, int update=1);
	FNUM PseudoDT1[6];
	FNUM PseudoDT2[6];
	static uint8_t carmsk[];
	static uint8_t opmap[];
public:
	COPL3(CPort* pt, int fsamp);
	virtual void Init();
};

class COPL3_2 : public CSpanDevice {
protected:
public:
	COPL3_2(CPort* pt1, CPort* pt2, uint8_t mode, int fsamp);
	virtual void Init() {};
};

#endif
