#pragma once
#ifndef __SCC_H__
#define __SCC_H__

#include "PSGBase.h"

class CSCCBase : public CPSGBase
{
protected:
	struct REGMAP {	//レジスタアドレス
		uint8_t device;
		uint8_t waveform;
		uint8_t frequency;
		uint8_t amplitude;
		uint8_t enable;
		uint8_t mode;
		uint8_t init;
		uint8_t bank;
		uint16_t offset;
		uint16_t config;
		uint16_t banksel;
	};
	REGMAP regmap;
	virtual void UpdateTL(uint8_t ch, uint8_t op, uint8_t lev);
	virtual void UpdateVolExp(uint8_t ch);
	virtual void UpdateFreq(uint8_t ch, const FNUM* fnum);
	virtual void UpdateVoice(uint8_t ch);
public:
	CSCCBase(CPort* pt, int fsamp, const REGMAP& regset);
	virtual void Init();
};

class CSCC : public CSCCBase
{
public:
	CSCC(CPort* pt, int fsamp);
protected:
	virtual void UpdateVoice(uint8_t ch);
};

class CSCCP : public CSCCBase
{
public:
	CSCCP(CPort* pt, int fsamp);
};

#endif
