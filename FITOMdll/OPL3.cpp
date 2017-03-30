#include "STDAFX.h"
#include "FITOM.h"
#include "OPL.h"
#include "MIDI.h"

UINT8 COPL3::opmap[] = {0x0, 0x3, 0x8, 0xb, };
UINT8 COPL3::carmsk[] = { 0x2, 0x3, 0x8, 0xc, 0x8, 0x9, 0xa, 0xd, 0xa, 0xe, 0xb, 0xf, 0xa, 0xe, 0xb, 0xf, };

#define GET_AR(v,o)	(v->op[o].AR >> 3)
#define GET_DR(v,o)	(v->op[o].DR >> 3)
#define GET_SR(v,o)	(v->op[o].SR >> 3)
#define GET_RR(v,o)	(v->op[o].RR >> 3)
#define GET_SL(v,o)	(v->op[o].SL >> 3)
#define GET_TL(v,o)	(v->op[o].TL >> 1)
#define GET_RV(v,o)	(v->op[o].REV >> 3)

COPL3::COPL3(CPort* pt, int fsamp) :
CSoundDevice(DEVICE_OPL3, 6, fsamp, 288, FNUM_OFFSET, FnumTableType::Fnumber, pt)
{
	SetReg(0x01, 0x20);	//Enable WS
	SetReg(0x105, 0x01);	//Enable OPL3
}

void COPL3::UpdateVoice(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	CMidiCh* parent = attr->GetParent();
	UINT8 mode = voice->AL & 0x0c;
	UINT16 rop = (ch > 2) ? 0x100 : 0;
	UINT8 dch = rop ? (ch-3) : ch;
	UINT8 tmp, i;

	for(i=0; i<4; i++)
	{
		tmp = ((voice->op[i].AM & 0x1) << 7) | ((voice->op[i].VIB & 0x1) << 6) |
			(GET_SR(voice, i)?0:0x20) | ((voice->op[i].KSR & 0x1) << 4) |
			(voice->op[i].MUL & 0x0F);
		SetReg(rop + 0x20 + opmap[i] + dch, tmp);

		tmp = (UINT8)((voice->op[i].KSL << 6) | GET_TL(voice, i));
		SetReg(rop + 0x40 + opmap[i] + dch, tmp);

		tmp = (GET_AR(voice, i) << 4) | GET_DR(voice, i);
		SetReg(rop + 0x60 + opmap[i] + dch, tmp);

		UINT8 rr = ((parent && parent->GetSustain() && (carmsk[voice->AL&0xf]&(1<<i))) ?
			GET_RV(voice, i) : (GET_SR(voice, i) ? GET_SR(voice, i) : GET_RR(voice, i))) & 0xf;
		tmp = (GET_SL(voice, i) << 4) | rr;
		SetReg(rop + 0x80 + opmap[i] + dch, tmp);

		SetReg(rop + 0xe0 + opmap[i] + dch, voice->op[i].WS);
	}
	tmp = (GetReg(rop + 0xc0 + dch, 0) & 0xf0) | ((voice->FB & 0x7) << 1) | (voice->AL & 0x1);
	SetReg(rop + 0xc0 + dch, tmp);
	tmp = (GetReg(rop + 0xc3 + dch, 0) & 0xf0) | ((voice->FB >> 3) & 0xe) | ((voice->AL >> 1) & 0x1);
	SetReg(rop + 0xc3 + dch, tmp);

	UINT8 con = GetReg(0x104, 0) & ~(1 << ch);
	if (mode & 0x04) {// CON=1
		SetReg(0x104, con | (1 << ch));
	} else {// CON=0
		SetReg(0x104, con);
	}
	UpdateVolExp(ch);
}

void COPL3::UpdateVolExp(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();

	UINT8 mode = voice->AL & 0x0c;
	UINT16 rop = (ch > 2) ? 0x100 : 0;
	UINT8 dch = rop ? (ch-3) : ch;
	UINT8 evol = attr->GetEffectiveLevel();
	for (int i=0; i<4; i++) {
		if ((1 << i) & carmsk[voice->AL & 0xf]) {
			UINT8 tl;
			UINT8 tmp;
			tl = CalcEffectiveLevel(evol, voice->op[i].TL);
			attr->baseTL[i] = tl;
			tl = (tl >= 64) ? 63 : (tl & 63);
			tmp = (UINT8)((voice->op[i].KSL << 6) | tl);
			SetReg(rop + 0x40 + opmap[i] + dch, tmp);
		}
	}
}

void COPL3::UpdatePanpot(UINT8 ch)
{
	int pan = (GetChAttribute(ch)->panpot) / 16;
	UINT8 chena = 0;
	if (pan >= 4) { //R
		chena = 0x20;
	}
	else if (pan <= -4) { //L
		chena = 0x10;
	}
	else {	//C
		chena = 0x30;
	}
	UINT16 rop = (ch > 2) ? 0x100 : 0;
	UINT8 dch = rop ? (ch-3) : ch;
	UINT8 tmp;
	tmp = GetReg(rop + 0xc0 + dch, 0);
	if ((tmp & 0xf0) != chena) {
		SetReg(rop + 0xc0 + dch, (tmp & 0x0f) | chena);
	}
	tmp = GetReg(rop + 0xc3 + dch, 0);
	if ((tmp & 0xf0) != chena) {
		SetReg(rop + 0xc3 + dch, (tmp & 0x0f) | chena);
	}
}

void COPL3::UpdateSustain(UINT8 ch)
{
	if (ch < chs) {
		CHATTR* attr = GetChAttribute(ch);
		FMVOICE* voice = attr->GetVoice();
		CMidiCh* parent = attr->GetParent();
		UINT8 mode = voice->AL & 0x0c;
		UINT16 rop = (ch > 2) ? 0x100 : 0;
		UINT8 dch = rop ? (ch-3) : ch;

		for (int i=0; i<4; i++) {
			if ((1 << i) & carmsk[voice->AL & 0xf]) {
				UINT8 rr = ((parent && parent->GetSustain() && (carmsk[voice->AL&0xf]&(1<<i))) ?
					GET_RV(voice, i) : (GET_SR(voice, i) ? GET_SR(voice, i) : GET_RR(voice, i))) & 0xf;
				UINT8 tmp = (GET_SL(voice, i) << 4) | rr;
				SetReg(rop + 0x80 + opmap[i] + dch, tmp);
			}
		}
	}
}

void COPL3::UpdateFnumber(UINT8 ch, int update)
{
	if (ch < chs) {
		CHATTR* attr = GetChAttribute(ch);
		FMVOICE* voice = attr->GetVoice();
		SINT32 off1 = (voice->op[0].DT2 | (voice->op[0].DT1 << 7));
		SINT32 off2 = (voice->op[2].DT2 | (voice->op[2].DT1 << 7));
		off1 = (off1 < 8192) ? off1 : (off1 - 16384);
		off2 = (off2 < 8192) ? off2 : (off2 - 16384);
		PseudoDT1[ch] = GetFnumber(ch, SINT16(off1));
		PseudoDT2[ch] = GetFnumber(ch, SINT16(off2));
	}
	CSoundDevice::UpdateFnumber(ch, update);
}

void COPL3::UpdateFreq(UINT8 ch, const FNUM* fnum)
{
	CHATTR* attr = GetChAttribute(ch);
	UINT16 rop = (ch > 2) ? 0x100 : 0;
	UINT8 dch = rop ? (ch-3) : ch;
	UINT16 efn1 = (PseudoDT1[ch].fnum >> 1) & 0x3ff;
	UINT16 efn2 = (PseudoDT2[ch].fnum >> 1) & 0x3ff;

	SetReg(rop + 0xa0 + dch, (UINT8)(efn1 & 0xff), 1);
	SetReg(rop + 0xb0 + dch, (GetReg(rop + 0xb0 + dch, 0) & 0x20) |
		(UINT8)((PseudoDT1[ch].block << 2) | (efn1 >> 8)), 1);
	SetReg(rop + 0xa3 + dch, (UINT8)(efn2 & 0xff), 1);
	SetReg(rop + 0xb3 + dch, (GetReg(rop + 0xb3 + dch, 0) & 0x20) |
		(UINT8)((PseudoDT2[ch].block << 2) | (efn2 >> 8)), 1);
}

void COPL3::UpdateKey(UINT8 ch, UINT8 keyon)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	CMidiCh* parent = attr->GetParent();

	UINT16 rop = (ch > 2) ? 0x100 : 0;
	UINT8 dch = rop ? (ch-3) : ch;
	UINT8 tmp = GetReg(rop + 0xb0 + dch, 0) & 0xdf;
	SetReg(rop + 0xb0 + dch, tmp | (keyon ? 0x20 : 0), 1);

	if ((voice->AL & 0x08) || !keyon) {
		tmp = GetReg(rop + 0xb3 + dch, 0) & 0xdf;
		SetReg(rop + 0xb3 + dch, tmp | (keyon ? 0x20 : 0), 1);
	}
	for (int i = 0; i < 4; i++) {
		tmp = (GetReg(rop + 0x20 + dch + opmap[i], 0)) & 0xdf;
		SetReg(0x20 + dch + opmap[i], tmp | (keyon ? 0 : 0x20), 1);
		BOOL isCarrier = (1 << i) & carmsk[voice->AL & 0xf];
		BOOL isSustained = (parent && parent->GetSustain());
		UINT8 rr = (isSustained && isCarrier) ? GET_RV(voice, i) : GET_RR(voice, i);
		UINT8 sr = keyon ? GET_SR(voice, i) : rr;

		UINT8 tmp = (GET_SL(voice, i) << 4) | sr;
		SetReg(rop + 0x80 + opmap[i] + dch, tmp);
	}
}

void COPL3::UpdateTL(UINT8 ch, UINT8 op, UINT8 lev)
{
	UINT16 rop = (ch > 2) ? 0x100 : 0;
	UINT8 dch = rop ? (ch-3) : ch;
	lev = (lev >= 64) ? 63 : (lev & 63);
	SetReg(rop + 0x40 + opmap[op] + dch, lev);
}

COPL3_2::COPL3_2(CPort* pt1, CPort* pt2, UINT8 mode, int fsamp) : CSpanDevice(NULL, NULL)
{
	COPL* opl1 = new COPL(pt1, fsamp/4, DEVICE_OPL3_2);
	COPL* opl2 = new COPL(pt2, fsamp/4, DEVICE_OPL3_2);
	if (mode == 0) { // OPL3 6ch(4OP)+6ch(2OP)
		for (int i=0; i<6; i++) {
			opl1->EnableCh(i, 0);
			opl2->EnableCh(i, 0);
		}
	}
	AddDevice(opl1);
	AddDevice(opl2);
	SetDevice(DEVICE_OPL3_2);
}
