#include "stdafx.h"
#include "SSG.h"

//-------------------------------
CSSG::CSSG(CPort* pt, int fsamp, uint8_t dev) : CPSGBase(dev, pt, 0x20, 3, fsamp)
{
	ops = 2;
}

void CSSG::Init()
{
	SetReg(0x07, 0x3f, 1);
}

void CSSG::UpdateVolExp(uint8_t ch)
{
	CHATTR* attr = GetChAttribute(ch);
	if (!(attr->GetVoice()->op[0].EGT & 0x8)) {
		uint8_t evol = attr->GetEffectiveLevel();
		int16_t lev = int16_t(lfoTL[ch]) - 64 + egattr[ch].GetValue();
		lev = (lev < 0) ? 0 : lev;
		lev = (lev > 127) ? 127 : lev;
		evol = 15 - Linear2dB(CalcLinearLevel(evol, ROM::VolCurveLin[uint8_t(lev)]), RANGE48DB, STEP300DB, 4);
		SetReg(8 + ch, evol & 0xf, 0);
	}
}

void CSSG::UpdateFreq(uint8_t ch, const FNUM* fnum)
{
	fnum = fnum ? fnum : GetChAttribute(ch)->GetLastFnumber();
	uint8_t oct = fnum->block;
	uint16_t etp = fnum->fnum >> (oct + 3);
	SetReg(ch * 2 + 0, uint8_t(etp & 0xff), 0);
	SetReg(ch * 2 + 1, uint8_t(etp >> 8), 0);
}

void CSSG::UpdateKey(uint8_t ch, uint8_t keyon)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	if (voice->op[0].EGT & 0x8) {
		if (keyon) {
			UpdateVoice(ch);
		}
		else {
			SetReg(0x8 + ch, (GetReg(0x8 + ch, 0) & 0xe0), 1);
		}
	}
	else {
		CPSGBase::UpdateKey(ch, keyon);
	}
}

void CSSG::EGOff(uint8_t ch)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	if (!(voice->op[0].EGT & 0x8)) {
		NoteOff(ch);
	}
}

void CSSG::UpdateVoice(uint8_t ch)
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
	SetReg(0x7, (GetReg(0x7, 0) & ~(0x9 << ch)) | (mix << ch), 1);
	if (mix==1 || mix==0) {
		SetReg(0x6, voice->NFQ, 1);
	}
	if (voice->op[0].EGT & 0x8) {	//HW env
		SetReg(0x8+ch, (GetReg(0x8+ch, 0) & 0xe0) | 0x10 | voice->op[0].EGT, 1);
		SetReg(0xb, (((voice->op[0].SL << 4) & 0xf0) | (voice->op[0].RR & 0xf)), 1);
		SetReg(0xc, (((voice->op[0].DR << 4) & 0xf0) | (voice->op[0].SR & 0xf)), 1);
		SetReg(0xd,  (voice->op[0].EGT & 0xf), 1);
	}
}

void CSSG::UpdateTL(uint8_t ch, uint8_t op, uint8_t lev)
{
	FMVOICE* voice = GetChAttribute(ch)->GetVoice();
	switch (op) {
	case 0:// Amplitude LFO
		{
			lfoTL[ch] = lev;
		}
		break;
	case 1:// Noise freq LFO
		if ((voice->AL & 3)==1 || (voice->AL & 3)==2) {
			int16_t frq = int16_t(lev) - 64 + (((voice->NFQ << 2) | (voice->NFQ >> 3)) & 0x7f);
			frq = (frq < 0) ? 0 : frq;
			frq = (frq > 127) ? 127 : frq;
			SetReg(0x6, lev >> 2, 0);
		}
		break;
	}
}

uint8_t CSSG::QueryCh(CMidiCh* parent, FMVOICE* voice, int mode)
{
	CHATTR* attr2 = GetChAttribute(2);
	CHATTR* attr1 = GetChAttribute(1);
	if (voice && (voice->AL & 0x3) != 0) {//Noise enabled
		if (mode ? attr2->IsAvailable() : attr2->IsEnable()) {
			return 2;
		} else {
			return 0xff;
		}
	/*
	} else if (voice && (voice->AL & 0x20) != 0) {//HW Envelop
		if (mode ? attr1->IsAvailable() : attr1->IsEnable()) {
			return 1;
		} else {
			return 0xff;
		}
	*/
	}
	return CSoundDevice::QueryCh(parent, voice, mode);
}
