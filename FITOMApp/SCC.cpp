#include "stdafx.h"
#include "SCC.h"

//-------------------------------
CSCCBase::CSCCBase(CPort* pt, int fsamp, const REGMAP& map)
	: CPSGBase(map.device, pt, 0x100, 5, fsamp), regmap(map)
{
	ops = 1;
}

void CSCCBase::UpdateVolExp(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	UINT8 evol = attr->GetEffectiveLevel();
	SINT16 lev = SINT16(lfoTL[ch]) - 64 + egattr[ch].GetValue();
	lev = (lev < 0) ? 0 : lev;
	lev = (lev > 127) ? 127 : lev;
	//evol = 15 - Linear2dB(CalcLinearLevel(evol, 127 - UINT8(lev)), RANGE48DB, STEP300DB, 4);
	evol = CalcLinearLevel(evol, 127 - UINT8(lev)) >> 3;
	SetReg(regmap.amplitude + ch, evol & 0xf, 0);
}

void CSCCBase::UpdateFreq(UINT8 ch, const FNUM* fnum)
{
	fnum = fnum ? fnum : GetChAttribute(ch)->GetLastFnumber();
	UINT8 oct = fnum->block;
	UINT16 etp = fnum->fnum >> (oct + 3);
	SetReg(ch * 2 + regmap.frequency + 0, UINT8(etp & 0xff), 0);
	SetReg(ch * 2 + regmap.frequency + 1, UINT8(etp >> 8), 0);
}

void CSCCBase::UpdateVoice(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	lfoTL[ch] = attr->baseTL[0] = 64;
	SCCWAVE wave;
	CFITOM::GetInstance()->GetWaveform(&wave, voice->op[0].WS);
	for (int i = 0; i < 32; i++) {
		int idx = (i + voice->FB) & 31;
		SetReg((ch * 32) + i + regmap.waveform, wave.wave[idx], 1);
	}
}

void CSCCBase::UpdateTL(UINT8 ch, UINT8 op, UINT8 lev)
{
	FMVOICE* voice = GetChAttribute(ch)->GetVoice();
	switch (op) {
	case 0:// Amplitude LFO
	{
		lfoTL[ch] = lev;
	}
	break;
	}
}

CSCC::CSCC(CPort* pt, int fsamp) : CSCCBase(pt, fsamp,
{ DEVICE_SCC, 0x00, 0x80, 0x8a, 0x8f, 0xc0, 0x00, 0x3f, 0x9800, 0xbffe, 0x9000 })
{
	pt->reset();
	pt->writeRaw(regmap.banksel, regmap.bank);
	GetChAttribute(4)->OutOfDVA();	// 5th ch should always be assigned manually
	for (int i = 0; i < chs; i++) {
		SetReg(regmap.amplitude + i, 0, 1);
		SetReg(regmap.frequency + i * 2, 0, 1);
		SetReg(regmap.frequency + i * 2 + 1, 0, 1);
	}
	SetReg(regmap.enable, 0x1f, 1);
	SetReg(0xc0, 0x00, 1);
	SetReg(0xe0, 0x00, 1);
}

void CSCC::UpdateVoice(UINT8 ch)
{
	if (ch < 4) {
		CSCCBase::UpdateVoice(ch);
	}
	else
	{
		CHATTR* attr = GetChAttribute(ch);
		lfoTL[ch] = attr->baseTL[0] = 64;
	}
}

//-------------------------------
CSCCP::CSCCP(CPort* pt, int fsamp) : CSCCBase(pt, fsamp,
{ DEVICE_SCCP, 0x00, 0xa0, 0xaa, 0xaf, 0xe0, 0x20, 0x80, 0xb800, 0xbffe, 0xb000 })
{
	pt->writeRaw(regmap.config, regmap.init);
	pt->writeRaw(regmap.banksel, regmap.bank);
	for (int i = 0; i < chs; i++) {
		SetReg(regmap.amplitude + i, 0, 1);
		SetReg(regmap.frequency + i * 2, 0, 1);
		SetReg(regmap.frequency + i * 2 + 1, 0, 1);
	}
	SetReg(regmap.enable, 0x1f, 1);
	SetReg(regmap.mode, 0x00, 1);
}

