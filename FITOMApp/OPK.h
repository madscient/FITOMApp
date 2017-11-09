#ifndef __OPK_H__
#define __OPK_H__

#include "FITOMdefine.h"
#include "SoundDev.h"

class COPK : public CSoundDevice {
protected:
	virtual void UpdateVolExp(UINT8 ch);
	virtual void UpdateVoice(UINT8 ch);
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum);
	virtual void UpdatePanpot(UINT8 ch);
	virtual void UpdateSustain(UINT8 ch);
	virtual void UpdateTL(UINT8 ch, UINT8 op, UINT8 lev);
	virtual void UpdateKey(UINT8 ch, UINT8 keyon);
	static const UINT8 chofs[8];
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
	UINT8 RhythmOnMap;
	UINT8 RhythmOffMap;
	//Updater
	virtual void UpdateVolExp(UINT8 ch);
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum = 0) {};
	virtual void UpdateVoice(UINT8 ch);
	virtual void UpdatePanpot(UINT8 ch) {};
	virtual void UpdateKey(UINT8 ch, UINT8 keyon);
public:
	virtual void Init();
	virtual UINT8 QueryCh(CMidiCh* parent, FMVOICE* voice, int mode);
	COPKRhythm(CSoundDevice* parent);
};

#endif //__OPK_H__