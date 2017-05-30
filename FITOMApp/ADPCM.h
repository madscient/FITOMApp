#ifndef __ADPCM_H__
#define __ADPCM_H__

#include "SoundDev.h"

struct ADPCMVOICE {
	TCHAR name[32];		//program name
	UINT32 staddr;		//Start address (bit unit)
	UINT32 length;		//length (bit unit)
	UINT32 loopstart;	//offset from start
	UINT32 loopend;		//offset from start
};

class CAdPcmBase : public CSoundDevice
{
protected:
	ADPCMVOICE adpcmvoice[128];
	size_t maxmem;
	size_t usedmem;
	size_t boundary;
	UINT32 fmaster;
	UINT8 parentdev;
	UINT8 fcode;

	virtual void UpdateVolExp(UINT8 ch) {};
	virtual void UpdateVoice(UINT8 ch) {};
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum) {};
	virtual void UpdatePanpot(UINT8 ch) {};
	virtual void UpdateSustain(UINT8 ch) {};
	virtual void UpdateKey(UINT8 ch, UINT8 keyon) {};
	virtual void UpdateTL(UINT8 ch, UINT8 op, UINT8 lev) {};
public:
	CAdPcmBase(UINT8 devid, CPort* pt, size_t regsize, int fsamp, int devide, int offset, size_t memsize, UINT8 maxch, UINT8 pardev);
	virtual UINT8 GetParentDev() { return parentdev; };
	virtual void LoadVoice(int prog, UINT8* data, size_t length) = 0;
	virtual void TimerCallBack(UINT32 tick) {};
	void Debug();
};


#endif
