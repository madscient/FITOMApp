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
};

class CYmDelta : public CAdPcmBase	//YAMAHA DeltaN ADPCM
{
protected:
	virtual ISoundDevice::FNUM GetFnumber(UINT8 ch, SINT16 offset);
	virtual void UpdateVolExp(UINT8 ch);
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum);
	virtual void UpdateKey(UINT8 ch, UINT8 keyon);
	virtual void UpdateVoice(UINT8 ch);
	virtual void UpdatePanpot(UINT8 ch);
	virtual void PlayPCM(UINT8 ch, UINT8 num, UINT8 volume, SINT8 pan, SINT32 offset);
	virtual void StopPCM(UINT8 ch, UINT8 num);
	virtual UINT16 GetDeltaN(int off);
	UINT8 chena;	// L/R
	struct REGMAP {	//レジスタアドレス
		UINT8 control1;
		UINT8 control2;
		UINT8 startLSB;
		UINT8 startMSB;
		UINT8 endLSB;
		UINT8 endMSB;
		UINT8 limitLSB;
		UINT8 limitMSB;
		UINT8 memory;
		UINT8 deltanLSB;
		UINT8 deltanMSB;
		UINT8 volume;
		UINT8 flag;
		UINT8 ctrl1init;
		UINT8 ctrl2init;
		UINT8 panmask;
	};
	REGMAP regmap;
public:
	CYmDelta(UINT8 devid, CPort* pt, size_t regsize, int fsamp, int devide, size_t memsize, UINT8 pardev, const REGMAP& regset);
	virtual UINT8 QueryCh(CMidiCh* parent, FMVOICE* voice, int mode) { return 0; };
	virtual void LoadVoice(int prog, UINT8* data, size_t length);
	virtual void RhythmOn(UINT8 num, UINT8 vel, SINT8 pan, FMVOICE* rv, FNUM* fnum);
	virtual void RhythmOff(UINT8 num);
};

class CAdPcm3801 : public CYmDelta	//Y8950
{
protected:
public:
	CAdPcm3801(CPort* pt, int fsamp, size_t memsize);
};

class CAdPcm2608 : public CYmDelta	//YM2608
{
protected:
public:
	CAdPcm2608(CPort* pt, int fsamp, size_t memsize);
};

class CAdPcm2610B : public CYmDelta//YM2610 ADPCM B
{
protected:
public:
	CAdPcm2610B(CPort* pt, int fsamp, size_t memsize, UINT8 pardev = DEVICE_OPNB);
	virtual void LoadVoice(int prog, UINT8* data, size_t length);
	virtual void UpdateVoice(UINT8 ch);
};

class CAdPcm2610A : public CAdPcmBase	//YM2610 ADPCM A
{
protected:
public:
	CAdPcm2610A(CPort* pt, int fsamp, size_t memsize, UINT8 pardev = DEVICE_OPNB);
	virtual UINT8 QueryCh(CMidiCh* parent, FMVOICE* voice, int mode);
	virtual void LoadVoice(int prog, UINT8* data, size_t length);
	virtual void RhythmOn(UINT8 num, UINT8 vel, SINT8 pan, FMVOICE* rv, FNUM* fnum);
	virtual void RhythmOff(UINT8 num);
	virtual void UpdateRhythmVol();
	virtual void UpdateVolExp(UINT8 ch);
	virtual void UpdateKey(UINT8 ch, UINT8 keyon);
	virtual void UpdateVoice(UINT8 ch);
	virtual void UpdatePanpot(UINT8 ch);
};

#if 0
class CAdPcm263 : public CAdPcmBase	//YMZ263
{
};

class CAdPcm705 : public CAdPcmBase	//YMZ705
{
};

#endif

class CAdPcmZ280 : public CAdPcmBase	//YAMAHA PCM8 ADPCM
{
protected:
	virtual ISoundDevice::FNUM GetFnumber(UINT8 ch, SINT16 offset);
	virtual void UpdateVolExp(UINT8 ch);
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum);
	virtual void UpdateKey(UINT8 ch, UINT8 keyon);
	virtual void UpdateVoice(UINT8 ch);
	virtual void UpdatePanpot(UINT8 ch);
	virtual UINT16 GetDeltaN(int off);
public:
	CAdPcmZ280(CPort* pt, int fsamp, size_t memsize);
	virtual UINT8 QueryCh(CMidiCh* parent, FMVOICE* voice, int mode) { return 0; };
	virtual void LoadVoice(int prog, UINT8* data, size_t length);
};

#endif
