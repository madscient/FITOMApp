#pragma once
#ifndef __SCC_H__
#define __SCC_H__

#include "PSGBase.h"

class CSCCBase : public CPSGBase
{
protected:
	struct REGMAP {	//レジスタアドレス
		UINT8 device;
		UINT8 waveform;
		UINT8 frequency;
		UINT8 amplitude;
		UINT8 enable;
		UINT8 mode;
		UINT8 init;
		UINT8 bank;
		UINT16 offset;
		UINT16 config;
		UINT16 banksel;
	};
	REGMAP regmap;
	virtual void UpdateTL(UINT8 ch, UINT8 op, UINT8 lev);
	virtual void UpdateVolExp(UINT8 ch);
	virtual void UpdateFreq(UINT8 ch, const FNUM* fnum);
	virtual void UpdateVoice(UINT8 ch);
public:
	CSCCBase(CPort* pt, int fsamp, const REGMAP& regset);
	virtual void Init();
};

class CSCC : public CSCCBase
{
public:
	CSCC(CPort* pt, int fsamp);
protected:
	virtual void UpdateVoice(UINT8 ch);
};

class CSCCP : public CSCCBase
{
public:
	CSCCP(CPort* pt, int fsamp);
};

#endif
