#include "STDAFX.h"
#include "OPN.h"

uint8_t COPN::map[] = {0, 8, 4, 12};
uint8_t COPN::carmsk[] = { 0x8, 0x8, 0x8, 0x8, 0xa, 0xe, 0xe, 0xf, };

#define GET_AR(v,o)	(v->op[o].AR >> 2)
#define GET_DR(v,o)	(v->op[o].DR >> 2)
#define GET_SR(v,o)	(v->op[o].SR >> 2)
#define GET_RR(v,o)	(v->op[o].RR >> 3)
#define GET_SL(v,o)	(v->op[o].SL >> 3)
#define GET_TL(v,o)	(v->op[o].TL)
#define GET_AM(v,o)	(v->op[o].AVF & 1)
#define GET_VIB(v,o) ((v->op[o].AVF >> 1) & 1)
#define GET_FIX(v,o) ((v->op[o].AVF >> 2) & 1)
#define GET_KSL(v,o) ((v->op[o].KS >> 4) & 3)
#define GET_KSR(v,o) (v->op[o].KS & 3)
#define GET_ML(v,o)	(v->op[o].ML & 0xf)
#define GET_RV(v,o)	(v->op[o].SRR & 0xf)

COPN::COPN(CPort* pt, int fsamp, uint8_t devtype) :
CSoundDevice(devtype, 3, fsamp, 144, FNUM_OFFSET, FnumTableType::Fnumber, pt, 0x100), fxena(true)
{
//	if (pt) { pt->reset(); }
}

void COPN::Init()
{
	if (device == DEVICE_OPN || device == DEVICE_OPNC) {
		// 1/6, 1/4
		SetReg(0x2f, 0, 1);
		SetReg(0x2d, 0, 1);
		SetReg(0x27, 0x0, 1);
	}
}

uint8_t COPN::QueryCh(CMidiCh* parent, FMVOICE* voice, int mode)
{
	CHATTR* attr = GetChAttribute(2);
	if (fxena && voice && (voice->AL & 8) != 0) {//FX mode enabled
		if (mode ? attr->IsAvailable() : attr->IsEnable()) {
			return 2;
		}
		else {
			return 0xff;
		}
	}
	return CSoundDevice::QueryCh(parent, voice, mode);
}

void COPN::UpdateVoice(uint8_t ch)
{
	uint8_t	tmp;
	uint8_t	i;
	uint8_t ex = 0;
	FMVOICE* voice = GetChAttribute(ch)->GetVoice();

	if (fxena && ch == 2) {
		SetReg(0x27, ((voice->AL & 8) << 4) | (GetReg(0x27) & 0x3f), 0);
	}
	tmp = (voice->FB << 3) | (voice->AL & 7);
	SetReg(0xb0 + ch, tmp);
	for (i=0; i<4; i++)
	{
		tmp = ((voice->op[i].DT1 & 0x7) << 4) | (voice->op[i].MUL & 0xf);
		SetReg(0x30 + map[i] + ch, tmp);
		SetReg(0x40 + map[i] + ch, GET_TL(voice, i));
		tmp = (GET_KSR(voice, i) << 6) | GET_AR(voice, i);
		SetReg(0x50 + map[i] + ch, tmp);
		tmp = (GET_AM(voice, i) << 7) | GET_DR(voice, i);
		SetReg(0x60 + map[i] + ch, tmp);
		SetReg(0x70 + map[i] + ch, GET_SR(voice, i));
		tmp = (GET_SL(voice, i) << 4) | GET_RR(voice, i);
		SetReg(0x80 + map[i] + ch, tmp);
		SetReg(0x90 + map[i] + ch, voice->op[i].WS);
	}
	UpdateVolExp(ch);
}

void COPN::UpdateVolExp(uint8_t ch)
{
	CHATTR* attr = GetChAttribute(ch);
	uint8_t evol = attr->GetEffectiveLevel();
	FMVOICE* voice = attr->GetVoice();
	for (int i=0; i<4; i++) {
		if (carmsk[voice->AL&7] & (1<<i)) {
			uint8_t tl = CalcLinearLevel(evol, voice->op[i].TL);
			attr->baseTL[i] = tl;
			tl = Linear2dB(tl, RANGE96DB, STEP075DB, 7);
			SetReg(0x40 + map[i] + ch, tl, 0);
		}
	}
}

void COPN::UpdateFnumber(uint8_t ch, int update)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	if (fxena && ch == 2 && (voice->AL & 8)) {
		for (int i = 0; i < 4; i++) {
			int32_t off = (voice->op[i].DT2 | (voice->op[i].DT3 << 7));
			off = off - 8192;
			PseudoDT[i] = GetFnumber(ch, int16_t(off));
		}
	}
	CSoundDevice::UpdateFnumber(ch, update);
}

void COPN::UpdateFreq(uint8_t ch, const FNUM* fnum)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	if (fxena && ch == 2 && (voice->AL & 8)) {
		SetReg(0xad, (PseudoDT[0].block << 3) | uint8_t(PseudoDT[0].fnum >> 8), 1);
		SetReg(0xa9, uint8_t(PseudoDT[0].fnum & 0xff), 1);
		SetReg(0xae, (PseudoDT[1].block << 3) | uint8_t(PseudoDT[1].fnum >> 8), 1);
		SetReg(0xaa, uint8_t(PseudoDT[1].fnum & 0xff), 1);
		SetReg(0xac, (PseudoDT[2].block << 3) | uint8_t(PseudoDT[2].fnum >> 8), 1);
		SetReg(0xa8, uint8_t(PseudoDT[2].fnum & 0xff), 1);
		SetReg(0xa6, (PseudoDT[3].block << 3) | uint8_t(PseudoDT[3].fnum >> 8), 1);
		SetReg(0xa2, uint8_t(PseudoDT[3].fnum & 0xff), 1);
	}
	else {
		fnum = fnum ? fnum : attr->GetLastFnumber();
		SetReg(0xa4 + ch, (fnum->block << 3) | uint8_t(fnum->fnum >> 8), 1);
		SetReg(0xa0 + ch, uint8_t(fnum->fnum & 0xff), 1);
	}
}

void COPN::UpdatePanpot(uint8_t ch)
{
	int pan = (GetChAttribute(ch)->GetPanpot()) / 8;
	uint8_t chena = 0;
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

void COPN::UpdateSustain(uint8_t ch)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	CMidiCh* parent = attr->GetParent();
	for (int i=0; i<4; i++) {
		uint8_t rr = (parent && parent->GetSustain() ? GET_RV(voice, i) : GET_RR(voice, i)) & 0xf;
		uint8_t tmp = (GET_SL(voice, i) << 4) | rr;
		SetReg(0x80 + map[i] + ch, tmp);
	}
}

void COPN::UpdateTL(uint8_t ch, uint8_t op, uint8_t lev)
{
	SetReg(0x40 + map[op] + ch, lev, 0);
}

void COPN::UpdateKey(uint8_t ch, uint8_t keyon)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	CMidiCh* parent = attr->GetParent();
	if (keyon && parent && parent->GetForceDamp()) {
		for (int i=0; i<4; i++) {
			uint8_t tmp = (GET_SL(voice, i) << 4) | 0xf;
			SetReg(0x80 + map[i] + ch, tmp);
		}
	} else {
		UpdateSustain(ch);
	}
	SetReg(0x28, (keyon ? 0xf0 : 0) | ch, 1);
#ifdef _DEBUG
	const FNUM* fnum = attr->GetLastFnumber();
	if (fnum) {
		uint8_t acth = GetReg(0xa4 + ch, 0);
		uint8_t actl = GetReg(0xa0 + ch, 0);
		uint8_t actblk = (acth >> 3) & 0x7;
		uint16_t actfnum = ((acth & 7) << 8) | actl;
		if (fnum->block != actblk || fnum->fnum != actfnum) {
			int oh_my_god = 1;
			fprintf(stderr, _T("Fnum not match: attr=%02X%04X, act=%02X%04X\n"), fnum->block, fnum->fnum, actblk, actfnum);
		}
	}
#endif
}



