#include "stdafx.h"
#include "EPSG.h"

//-------------------------------
CEPSG::CEPSG(CPort* pt, int fsamp) : CPSGBase(DEVICE_EPSG, pt, 0x20, 3, fsamp), prevmix(0x3f)
{
	ops = 2;
	for (int i = 0; i < 3; i++) {
		prevvol[i] = 0;
		prevfreq[i] = 0;
	}
}

void CEPSG::Init()
{
	SetReg(0xd, 0xb0, 1);	//Enable expand mode
	SetReg(0x9, 0xff, 1);	//Noise "AND" mask
	SetReg(0xa, 0x00, 1);	//Noise "OR" mask
	SetReg(0x04, 0, 1);
	SetReg(0x05, 0, 1);
	SetReg(0xd, 0xa0, 1);	//Bank select A
	SetReg(0x07, 0x3f, 1);
	SetReg(0x08, 0, 1);
	SetReg(0x09, 0, 1);
	SetReg(0x0a, 0, 1);
}

void CEPSG::UpdateVolExp(uint8_t ch)
{
	CHATTR* attr = GetChAttribute(ch);
	if (!(attr->GetVoice()->FB & 0x8)) {
		uint8_t evol = attr->GetEffectiveLevel();
		int16_t lev = int16_t(lfoTL[ch]) - 64 + egattr[ch].GetValue();
		lev = (lev < 0) ? 0 : lev;
		lev = (lev > 127) ? 127 : lev;
		evol = 31 - Linear2dB(CalcLinearLevel(evol, ROM::VolCurveLin[uint8_t(lev)]), RANGE48DB, STEP150DB, 5);
		if (evol != prevvol[ch]) {
			prevvol[ch] = evol;
			//SetReg(0xd, 0xa0 | (GetReg(0xd, 0) & 0xf), 1);	//Bank select A
			SetReg(8 + ch, evol & 0x1f, 1);
		}
	}
}

void CEPSG::UpdateKey(uint8_t ch, uint8_t keyon)
{
	CPSGBase::UpdateKey(ch, keyon);
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	if (voice->FB & 0x8) {
		if (keyon) {
			UpdateVoice(ch);
		}
		else {
			SetReg(0x8 + ch, (GetReg(0x8 + ch, 0) & 0xc0), 1);
		}
	}
	else {
		CPSGBase::UpdateKey(ch, keyon);
	}
}

void CEPSG::EGOff(uint8_t ch)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	if (!(voice->FB & 0x8)) {
		NoteOff(ch);
	}
}

void CEPSG::UpdateFreq(uint8_t ch, const FNUM* fnum)
{
	fnum = fnum ? fnum : GetChAttribute(ch)->GetLastFnumber();
	uint8_t oct = fnum->block;
	uint16_t etp = fnum->fnum >> (oct + 2);
	if (etp != prevfreq[ch]) {
		prevfreq[ch] = etp;
		//SetReg(0xd, 0xa0 | (GetReg(0xd, 0) & 0xf), 1);	//Bank select A
		SetReg(ch * 2 + 0, uint8_t(etp & 0xff), 1);
		SetReg(ch * 2 + 1, uint8_t(etp >> 8), 1);
	}
}

void CEPSG::UpdateVoice(uint8_t ch)
{
	uint8_t mix = 8;
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	switch (voice->AL & 3) {
	case 0:
		mix = 0x8;
		break;
	case 1:
		mix = 0x1;
		break;
	case 2:
		mix = 0x0;
		break;
	case 3:
		mix = 0x9;
		break;
	}
	lfoTL[ch] = attr->baseTL[0] = attr->baseTL[1] = 64;
	//SetReg(0xd, 0xa0 | (GetReg(0xd, 0) & 0xf), 1);	//Bank select A
	prevmix = (prevmix & ~(0x9 << ch)) | (mix << ch);
	SetReg(0x7, prevmix, 1);
	if (mix == 1 || mix == 0) {
		SetReg(0x6, (voice->NFQ) | ((voice->FB & 0x7) << 5), 1);
	}

	bool hwenv23 = false;
	if (voice->FB & 0x8) {	//HW env
		SetReg(0x8 + ch, 0x20, 1);
		if (ch == 0) {
			SetReg(0xd, 0xa0 | (voice->FB & 0xf), 1);
			SetReg(0xb, (((voice->op[0].SL << 4) & 0xf0) | (voice->op[0].RR & 0xf)), 1);
			SetReg(0xc, (((voice->op[0].DR << 4) & 0xf0) | (voice->op[0].SR & 0xf)), 1);
		}
		else {
			hwenv23 = true;
		}
	}
	//Duty ratio
	SetReg(0xd, 0xb0 | (GetReg(0xd, 0) & 0xf), 1);	//Bank select B
	SetReg(0x6 + ch, voice->op[0].WS & 0xf, 1);
	if (hwenv23) {
		SetReg(0x0 + (ch - 1) * 2, (((voice->op[0].SL << 4) & 0xf0) | (voice->op[0].RR & 0xf)), 1);
		SetReg(0x1 + (ch - 1) * 2, (((voice->op[0].DR << 4) & 0xf0) | (voice->op[0].SR & 0xf)), 1);
		SetReg(0x3 + ch, (voice->FB & 0xf), 1);
	}
	SetReg(0xd, 0xa0 | (GetReg(0xd, 0) & 0xf), 1);	//Bank select A
}

void CEPSG::UpdateTL(uint8_t ch, uint8_t op, uint8_t lev)
{
	FMVOICE* voice = GetChAttribute(ch)->GetVoice();
	switch (op) {
	case 0:// Amplitude LFO
	{
		lfoTL[ch] = lev;
	}
	break;
	case 1:// Noise freq LFO
		if ((voice->AL & 3) == 1 || (voice->AL & 3) == 2) {
			int16_t frq = int16_t(lev) - 64 + (((voice->NFQ << 2) | (voice->FB >> 1)) & 0x7f);
			frq = (frq < 0) ? 0 : frq;
			frq = (frq > 255) ? 255 : frq;
			//SetReg(0xd, 0xa0 | (GetReg(0xd, 0) | 0xf), 1);	//Bank select A
			SetReg(0x6, lev, 1);
		}
		break;
	}
}

