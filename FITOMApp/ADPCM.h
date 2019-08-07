#ifndef __ADPCM_H__
#define __ADPCM_H__

#include "SoundDev.h"

struct ADPCMVOICE {
	TCHAR name[32];		//program name
	uint32_t staddr;		//Start address (bit unit)
	uint32_t length;		//length (bit unit)
	uint32_t loopstart;	//offset from start
	uint32_t loopend;		//offset from start
};

class CAdPcmBase : public CSoundDevice
{
protected:
	ADPCMVOICE adpcmvoice[128];
	size_t maxmem;
	size_t usedmem;
	size_t boundary;
	uint32_t fmaster;
	uint8_t parentdev;
	uint8_t fcode;

	virtual void UpdateVolExp(uint8_t ch) {};
	virtual void UpdateVoice(uint8_t ch) {};
	virtual void UpdateFreq(uint8_t ch, const FNUM* fnum) {};
	virtual void UpdatePanpot(uint8_t ch) {};
	virtual void UpdateSustain(uint8_t ch) {};
	virtual void UpdateKey(uint8_t ch, uint8_t keyon) {};
	virtual void UpdateTL(uint8_t ch, uint8_t op, uint8_t lev) {};
public:
	CAdPcmBase(uint8_t devid, CPort* pt, size_t regsize, int fsamp, int devide, int offset, size_t memsize, uint8_t maxch, uint8_t pardev);
	virtual void Init() = 0;
	virtual uint8_t GetParentDev() { return parentdev; };
	virtual void LoadVoice(int prog, uint8_t* data, size_t length) = 0;
	void Debug();
};


#endif
