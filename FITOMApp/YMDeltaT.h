#pragma once
#ifndef __YMDELTAT_H__
#define __YMDELTAT_H__

#include "ADPCM.h"

class CYmDelta : public CAdPcmBase	//YAMAHA DeltaT ADPCM
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
	virtual void LoadVoice(int prog, UINT8* data, size_t length);
	virtual void RhythmOn(UINT8 num, UINT8 vel, SINT8 pan, FMVOICE* rv, FNUM* fnum);
	virtual void RhythmOff(UINT8 num);
	virtual void UpdateRhythmVol();
	virtual void UpdateVolExp(UINT8 ch);
	virtual void UpdateKey(UINT8 ch, UINT8 keyon);
	virtual void UpdateVoice(UINT8 ch);
	virtual void UpdatePanpot(UINT8 ch);
};

#endif


