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
	UINT8 GetRhythmCh(UINT8 num);
	UINT8 RhythmStat[4];
	UINT8 AllocRhythmCh(UINT8 num);
	static const UINT8 chofs[8];
public:
	COPK(CPort* pt, int fsamp);
	virtual void Init();
	virtual void RhythmOn(UINT8 num, UINT8 vel, SINT8 pan, FMVOICE* rv, FNUM* fnum);
	virtual void RhythmOff(UINT8 num);
};

class COPK2 : public COPK {
public:
	COPK2(CPort* pt, int fsamp);
};

#endif //__OPK_H__