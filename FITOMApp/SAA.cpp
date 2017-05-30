#include "stdafx.h"
#include "SAA.h"

//-------------------------------
CSAA::CSAA(CPort* pt, int fsamp) : CPSGBase(DEVICE_SAA, pt, 0x20, 6, fsamp, 1024 * 256, -640, FnumTableType::saa)
{
	ops = 1;
}

void CSAA::UpdateVolExp(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	if (!(attr->GetVoice()->op[0].EGT & 0x8)) {
		UINT8 evol = attr->GetEffectiveLevel();
		SINT16 lev = SINT16(lfoTL[ch]) - 64 + egattr[ch].GetValue();
		lev = (lev < 0) ? 0 : lev;
		lev = (lev > 127) ? 127 : lev;
		evol = 15 - Linear2dB(CalcLinearLevel(evol, 127 - UINT8(lev)), RANGE48DB, STEP300DB, 4);
		SetReg(0xaa + ch, evol & 0xf, 1);
	}
}

void CSAA::UpdateFreq(UINT8 ch, const FNUM* fnum)
{
	fnum = fnum ? fnum : GetChAttribute(ch)->GetLastFnumber();
	UINT8 oct = fnum->block;
	UINT16 etp = fnum->fnum >> (oct + 2);
	SetReg(ch * 2 + 0xa0, UINT8(etp & 0xff), 1);
	SetReg(ch * 2 + 0xa1, UINT8(etp >> 8), 1);
}

void CSAA::UpdateVoice(UINT8 ch)
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

void CSAA::UpdateTL(UINT8 ch, UINT8 op, UINT8 lev)
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
