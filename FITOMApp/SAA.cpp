#include "stdafx.h"
#include "SAA.h"

//-------------------------------
CSAA::CSAA(CPort* pt, int fsamp) : CPSGBase(DEVICE_SAA, pt, 0x20, 6, fsamp, 1024 * 256, -640, FnumTableType::saa)
{
	ops = 1;
}

void CSAA::Init()
{
}

void CSAA::UpdateVolExp(uint8_t ch)
{
	CHATTR* attr = GetChAttribute(ch);
	if (!(attr->GetVoice()->op[0].EGT & 0x8)) {
		uint8_t evol = attr->GetEffectiveLevel();
		int16_t lev = int16_t(lfoTL[ch]) - 64 + egattr[ch].GetValue();
		lev = (lev < 0) ? 0 : lev;
		lev = (lev > 127) ? 127 : lev;
		evol = 15 - Linear2dB(CalcLinearLevel(evol, 127 - uint8_t(lev)), RANGE48DB, STEP300DB, 4);
		SetReg(0xaa + ch, evol & 0xf, 1);
	}
}

void CSAA::UpdateFreq(uint8_t ch, const FNUM* fnum)
{
	fnum = fnum ? fnum : GetChAttribute(ch)->GetLastFnumber();
	uint8_t oct = fnum->block;
	uint16_t etp = fnum->fnum >> (oct + 2);
	SetReg(ch * 2 + 0xa0, uint8_t(etp & 0xff), 1);
	SetReg(ch * 2 + 0xa1, uint8_t(etp >> 8), 1);
}

void CSAA::UpdateVoice(uint8_t ch)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	lfoTL[ch] = attr->baseTL[0] = 64;
	SCCWAVE wave;
	CFITOM::GetInstance()->GetWaveform(&wave, voice->op[0].WS);
	for (int i = 0; i < 32; i++) {
		int idx = (i + voice->FB) & 31;
		SetReg((ch * 32) + i, wave.wave[idx], 1);
	}
}

void CSAA::UpdateTL(uint8_t ch, uint8_t op, uint8_t lev)
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

void CSAA::SetReg(uint16_t reg, uint8_t data, uint8_t v)
{
	if (!v && regbak) {
		v = (regbak[reg] != data);
	}
	if (regbak) {
		regbak[reg] = data;
	}
	if (v && port) {
		port->writeRaw(0x1, reg);
		port->writeRaw(0x0, data);
	}
}