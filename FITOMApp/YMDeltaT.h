#pragma once
#ifndef __YMDELTAT_H__
#define __YMDELTAT_H__

#include "FITOMdefine.h"
#include "ADPCM.h"

class CYmDelta : public CAdPcmBase	//YAMAHA DeltaT ADPCM
{
protected:
	virtual ISoundDevice::FNUM GetFnumber(uint8_t ch, int16_t offset);
	virtual void UpdateVolExp(uint8_t ch);
	virtual void UpdateFreq(uint8_t ch, const FNUM* fnum);
	virtual void UpdateKey(uint8_t ch, uint8_t keyon);
	virtual void UpdateVoice(uint8_t ch);
	virtual void UpdatePanpot(uint8_t ch);
	virtual void PlayPCM(uint8_t ch, uint8_t num, uint8_t volume, int8_t pan, int32_t offset);
	virtual void StopPCM(uint8_t ch, uint8_t num);
	virtual uint16_t GetDeltaN(int off);
	uint8_t chena;	// L/R
	struct REGMAP {	//レジスタアドレス
		uint8_t control1;
		uint8_t control2;
		uint8_t startLSB;
		uint8_t startMSB;
		uint8_t endLSB;
		uint8_t endMSB;
		uint8_t limitLSB;
		uint8_t limitMSB;
		uint8_t memory;
		uint8_t deltanLSB;
		uint8_t deltanMSB;
		uint8_t volume;
		uint8_t flag;
		uint8_t ctrl1init;
		uint8_t ctrl2init;
		uint8_t panmask;
	};
	REGMAP regmap;
public:
	CYmDelta(uint8_t devid, CPort* pt, size_t regsize, int fsamp, int devide, size_t memsize, uint8_t pardev, const REGMAP& regset);
	virtual void Init() = 0;
	virtual uint8_t QueryCh(CMidiCh* parent, FMVOICE* voice, int mode) { return 0; };
	virtual void LoadVoice(int prog, uint8_t* data, size_t length);
	virtual void TimerCallBack(uint32_t tick) {};
};

class CAdPcm3801 : public CYmDelta	//Y8950
{
protected:
public:
	CAdPcm3801(CPort* pt, int fsamp, size_t memsize);
	virtual void Init();
};

class CAdPcm2608 : public CYmDelta	//YM2608
{
protected:
public:
	CAdPcm2608(CPort* pt, int fsamp, size_t memsize);
	virtual void Init();
};

class CAdPcm2610B : public CYmDelta//YM2610 ADPCM B
{
protected:
public:
	CAdPcm2610B(CPort* pt, int fsamp, size_t memsize, uint8_t pardev = DEVICE_OPNB);
	virtual void Init();
	virtual void LoadVoice(int prog, uint8_t* data, size_t length);
	virtual void UpdateVoice(uint8_t ch);
};

class CAdPcm2610A : public CAdPcmBase	//YM2610 ADPCM A
{
protected:
public:
	CAdPcm2610A(CPort* pt, int fsamp, size_t memsize, uint8_t pardev = DEVICE_OPNB);
	virtual void Init();
	virtual void LoadVoice(int prog, uint8_t* data, size_t length);
	virtual void UpdateVolExp(uint8_t ch);
	virtual void UpdateKey(uint8_t ch, uint8_t keyon);
	virtual void UpdateVoice(uint8_t ch);
	virtual void UpdatePanpot(uint8_t ch);
	virtual uint8_t QueryCh(CMidiCh* parent, FMVOICE* voice, int mode) { return CSoundDevice::QueryCh(parent, voice, 0); };
};

#endif


