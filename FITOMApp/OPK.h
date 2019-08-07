#ifndef __OPK_H__
#define __OPK_H__

#include "FITOMdefine.h"
#include "SoundDev.h"

class COPK : public CSoundDevice {
protected:
	virtual void UpdateVolExp(uint8_t ch);
	virtual void UpdateVoice(uint8_t ch);
	virtual void UpdateFreq(uint8_t ch, const FNUM* fnum);
	virtual void UpdatePanpot(uint8_t ch);
	virtual void UpdateSustain(uint8_t ch);
	virtual void UpdateTL(uint8_t ch, uint8_t op, uint8_t lev);
	virtual void UpdateKey(uint8_t ch, uint8_t keyon);
	static const uint8_t chofs[8];
public:
	COPK(CPort* pt, int fsamp);
	virtual void Init();
};

class COPK2 : public COPK {
public:
	COPK2(CPort* pt, int fsamp);
};

class COPKRhythm : public CRhythmDevice {
protected:
	uint8_t RhythmOnMap;
	uint8_t RhythmOffMap;
	//Updater
	virtual void UpdateVolExp(uint8_t ch);
	virtual void UpdateFreq(uint8_t ch, const FNUM* fnum = 0) {};
	virtual void UpdateVoice(uint8_t ch);
	virtual void UpdatePanpot(uint8_t ch) {};
	virtual void UpdateKey(uint8_t ch, uint8_t keyon);
public:
	virtual void Init();
	virtual uint8_t QueryCh(CMidiCh* parent, FMVOICE* voice, int mode);
	COPKRhythm(COPK* parent);
};

#endif //__OPK_H__