#include "stdafx.h"
#include "SSG.h"

//-------------------------------
CSSG::CSSG(CPort* pt, int fsamp) : CPSGBase(DEVICE_SSG, pt, 0x20, 3, fsamp)
{
	SetReg(0x07, 0x3f, 1);
	ops = 2;
}

void CSSG::UpdateVolExp(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	if (!(attr->GetVoice()->op[0].EGT & 0x8)) {
		UINT8 evol = attr->GetEffectiveLevel();
		SINT16 lev = SINT16(lfoTL[ch]) - 64 + egattr[ch].GetValue();
		lev = (lev < 0) ? 0 : lev;
		lev = (lev > 127) ? 127 : lev;
		evol = 15 - Linear2dB(CalcLinearLevel(evol, ROM::VolCurveLin[UINT8(lev)]), RANGE48DB, STEP300DB, 4);
		SetReg(8 + ch, evol & 0xf, 0);
	}
}

void CSSG::UpdateFreq(UINT8 ch, const FNUM* fnum)
{
	fnum = fnum ? fnum : GetChAttribute(ch)->GetLastFnumber();
	UINT8 oct = fnum->block;
	UINT16 etp = fnum->fnum >> (oct + 3);
	SetReg(ch * 2 + 0, UINT8(etp & 0xff), 0);
	SetReg(ch * 2 + 1, UINT8(etp >> 8), 0);
}

void CSSG::UpdateKey(UINT8 ch, UINT8 keyon)
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

void CSSG::EGOff(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	if (!(voice->op[0].EGT & 0x8)) {
		NoteOff(ch);
	}
}

void CSSG::UpdateVoice(UINT8 ch)
{
	UINT8 mix = 8;
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

void CSSG::UpdateTL(UINT8 ch, UINT8 op, UINT8 lev)
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
			SINT16 frq = SINT16(lev) - 64 + (((voice->NFQ << 2) | (voice->NFQ >> 3)) & 0x7f);
			frq = (frq < 0) ? 0 : frq;
			frq = (frq > 127) ? 127 : frq;
			SetReg(0x6, lev >> 2, 0);
		}
		break;
	}
}

UINT8 CSSG::QueryCh(CMidiCh* parent, FMVOICE* voice, int mode)
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

//-------------------------------
CPSG::CPSG(CPort* pt, int fsamp) : CSSG(pt, fsamp)
{
	SetDevice(DEVICE_PSG);
}

