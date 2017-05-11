#include "STDAFX.h"
#include "FITOM.h"
#include "OPN.h"
#include "MIDI.h"

UINT8 COPN::map[] = {0, 8, 4, 12};
UINT8 COPN::carmsk[] = { 0x8, 0x8, 0x8, 0x8, 0xa, 0xe, 0xe, 0xf, };

#define GET_AR(v,o)	(v->op[o].AR >> 2)
#define GET_DR(v,o)	(v->op[o].DR >> 2)
#define GET_SR(v,o)	(v->op[o].SR >> 2)
#define GET_RR(v,o)	(v->op[o].RR >> 3)
#define GET_SL(v,o)	(v->op[o].SL >> 3)
#define GET_TL(v,o)	(v->op[o].TL)
#define GET_RV(v,o)	(v->op[o].REV >> 3)

COPN::COPN(CPort* pt, int fsamp, UINT8 devtype) :
CSoundDevice(devtype, 3, fsamp, 144, FNUM_OFFSET, FnumTableType::Fnumber, pt, 0x100)
{
	// 1/6, 1/4
	SetReg(0x2f, 0, 1);
	SetReg(0x2d, 0, 1);
	SetReg(0x27, 0x0, 1);
}

void COPN::UpdateVoice(UINT8 ch)
{
	UINT8	tmp;
	UINT8	i;
	UINT8 ex = 0;
	FMVOICE* voice = GetChAttribute(ch)->GetVoice();

	tmp = (voice->FB << 3) | (voice->AL & 7);
	SetReg(0xb0 + ch, tmp);
	for (i=0; i<4; i++)
	{
		tmp = ((voice->op[i].DT1 & 0x7) << 4) | (voice->op[i].MUL & 0xf);
		SetReg(0x30 + map[i] + ch, tmp);
		SetReg(0x40 + map[i] + ch, GET_TL(voice, i));
		tmp = ((voice->op[i].KSR & 0x3) << 6) | GET_AR(voice, i);
		SetReg(0x50 + map[i] + ch, tmp);
		tmp = ((voice->op[i].AM & 0x1) << 7) | GET_DR(voice, i);
		SetReg(0x60 + map[i] + ch, tmp);
		SetReg(0x70 + map[i] + ch, GET_SR(voice, i));
		tmp = (GET_SL(voice, i) << 4) | GET_RR(voice, i);
		SetReg(0x80 + map[i] + ch, tmp);
	}
	UpdateVolExp(ch);
}

void COPN::UpdateVolExp(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	UINT8 evol = attr->GetEffectiveLevel();
	FMVOICE* voice = attr->GetVoice();
	for (int i=0; i<4; i++) {
		if (carmsk[voice->AL&7] & (1<<i)) {
			UINT8 tl = CalcLinearLevel(evol, voice->op[i].TL);
			attr->baseTL[i] = tl;
			tl = Linear2dB(tl, RANGE96DB, STEP075DB, 7);
			SetReg(0x40 + map[i] + ch, tl);
		}
	}
}

void COPN::UpdateFreq(UINT8 ch, const FNUM* fnum)
{
	fnum = fnum ? fnum : GetChAttribute(ch)->GetLastFnumber();
	SetReg(0xa4 + ch, (fnum->block << 3) | UINT8(fnum->fnum >> 8), 1);
	SetReg(0xa0 + ch, UINT8(fnum->fnum & 0xff), 1);
}

void COPN::UpdatePanpot(UINT8 ch)
{
	int pan = (GetChAttribute(ch)->panpot) / 8;
	UINT8 chena = 0;
	if (pan >= 4) { //R
		chena = 0x40;
	}
	else if (pan <= -4) { //L
		chena = 0x80;
	}
	else { //C
		chena = 0xc0;
	}
	if ((GetReg(0xb4 + ch, 0) & 0xc0) != chena) {
		SetReg(0xb4 + ch, (GetReg(0xb4 + ch, 0) & 0x3f) | chena);
	}
}

void COPN::UpdateSustain(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	CMidiCh* parent = attr->GetParent();
	for (int i=0; i<4; i++) {
		UINT8 rr = (parent && parent->GetSustain() ? GET_RV(voice, i) : GET_RR(voice, i)) & 0xf;
		UINT8 tmp = (GET_SL(voice, i) << 4) | rr;
		SetReg(0x80 + map[i] + ch, tmp);
	}
}

void COPN::UpdateTL(UINT8 ch, UINT8 op, UINT8 lev)
{
	SetReg(0x40 + map[op] + ch, lev);
}

void COPN::UpdateKey(UINT8 ch, UINT8 keyon)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	CMidiCh* parent = attr->GetParent();
	if (keyon && parent && parent->GetForceDamp()) {
		for (int i=0; i<4; i++) {
			UINT8 tmp = (GET_SL(voice, i) << 4) | 0xf;
			SetReg(0x80 + map[i] + ch, tmp);
		}
	} else {
		UpdateSustain(ch);
	}
	SetReg(0x28, (keyon ? 0xf0 : 0) | ch, 1);
}

//-----
COPNC::COPNC(CPort* pt, int fsamp) : COPN(pt, fsamp, DEVICE_OPNC)
{
}
