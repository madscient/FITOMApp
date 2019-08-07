#include "STDAFX.H"
#include "OPK.h"

const uint8_t COPK::chofs[] = {0, 1, 4, 5, 8, 9, 12, 13, };

#define GET_AR(v,o)	(v->op[o].AR >> 3)
#define GET_DR(v,o)	(v->op[o].DR >> 3)
#define GET_SR(v,o)	(v->op[o].SR >> 3)
#define GET_RR(v,o)	(v->op[o].RR >> 3)
#define GET_SL(v,o)	(v->op[o].SL >> 3)
#define GET_TL(v,o)	(v->op[o].TL)
//#define GET_RV(v,o)	(v->op[o].REV >> 3)
#define GET_RV(v,o)	(4)

COPK::COPK(CPort* pt, int fsamp)
	: CSoundDevice(DEVICE_OPK, 8, fsamp, 108, FNUM_OFFSET, FnumTableType::Fnumber, pt, 0x90)
{
	ops = 2;
	NoteOffset = -69;
}

void COPK::Init()
{
	SetReg(0x01, 0xff, 1);
	SetReg(0x84, 0x00, 1);
}

void COPK::UpdateVoice(uint8_t ch)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	SetReg(0x70 + ch, (GetReg(0x70 + ch, 0) & 0xf8) | (voice->FB & 0x7), 1);
	for (int i=0; i<2; i++) {
		uint8_t tmp;
		tmp = (GetReg(0x10 + chofs[ch] + (i * 2), 0) & 0x90) | voice->op[i].MUL;
		SetReg(0x10 + chofs[ch] + (i * 2), tmp | (voice->op[i].VIB ? 0x40 : 0) | (voice->op[i].WS ? 0x20 : 0), 1);
		tmp = GET_TL(voice, i);
		if (!i) {
			//tmp = (tmp << 1) | (voice->NFQ ? 1: 0);
		}
		SetReg(0x20 + chofs[ch] + (i * 2), tmp);
		SetReg(0x30 + chofs[ch] + (i * 2), (GET_AR(voice, i) << 4) | GET_DR(voice, i));
		SetReg(0x40 + chofs[ch] + (i * 2), (GET_SL(voice, i) << 4) | GET_SR(voice, i));
	}
	UpdateVolExp(ch);
}

void COPK::UpdateVolExp(uint8_t ch)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	uint8_t evol = CalcLinearLevel(attr->GetEffectiveLevel(), voice->op[1].TL);
	attr->baseTL[1] = evol;
	evol = Linear2dB(evol, RANGE96DB, STEP075DB, 7);
	SetReg(0x22 + chofs[ch], evol & 0x7f, 0);
}

void COPK::UpdateFreq(uint8_t ch, const FNUM* fnum)
{
	CHATTR* attr = GetChAttribute(ch);
	fnum = fnum ? fnum : attr->GetLastFnumber();
	uint8_t blk = fnum->block;
	uint16_t fn = fnum->fnum;
	if (fn & 0x800) {
		fn >>= 1;
		blk++;
	}
	SetReg(0x52 + chofs[ch], (blk << 5) | uint8_t(fn >> 6), 1);
	SetReg(0x62 + chofs[ch], uint8_t(fn << 2) & 0xf0, 1);
}

void COPK::UpdateTL(uint8_t ch, uint8_t op, uint8_t lev)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	if (op == 0) {
		SetReg(0x20 + chofs[ch], (lev << 1) | (voice->NFQ ? 1 : 0), 1);
	} else if (op == 1) {
		SetReg(0x22 + chofs[ch], (GetReg(0x22 + chofs[ch], 0) & 0x80) | lev, 0);
	}
}

void COPK::UpdateKey(uint8_t ch, uint8_t keyon)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	SetReg(0x40 + chofs[ch], (GET_SL(voice, 0) << 4) | (keyon ? GET_SR(voice, 0) : GET_RR(voice, 0)), 1);
	SetReg(0x42 + chofs[ch], (GET_SL(voice, 1) << 4) | (keyon ? GET_SR(voice, 1) : GET_RR(voice, 1)), 1);
	SetReg(0x12 + chofs[ch], (GetReg(0x12 + chofs[ch], 0) & 0x7f) | (keyon ? 0x80 : 0), 1);
}

void COPK::UpdateSustain(uint8_t ch)
{
}

void COPK::UpdatePanpot(uint8_t ch)
{
	int pan = (GetChAttribute(ch)->GetPanpot()) / 8;
	uint8_t chena = 0;
	if (pan >= 4) { //R
		chena = 0x40;
	}
	else if (pan <= -4) { //L
		chena = 0x20;
	}
	else {	//C
		chena = 0x60;
	}
	if ((GetReg(0x70 + ch, 0) & 0x60) != chena) {
		SetReg(0x70 + ch, (GetReg(0x70 + ch, 0) & 0x9f) | chena, 1);
	}
}

COPK2::COPK2(CPort* pt, int fsamp) : COPK(pt, fsamp)
{
	SetDevice(DEVICE_OPK2);
}


//-----
COPKRhythm::COPKRhythm(COPK* parent) : CRhythmDevice(parent, DEVICE_OPK_RHY, 4)
{
}

void COPKRhythm::Init()
{
}

void COPKRhythm::UpdateVolExp(uint8_t ch)
{
	CHATTR* attr = GetChAttribute(ch);
	if (attr) {
		uint8_t evol = Linear2dB(CalcLinearLevel(attr->velocity, 127 - attr->GetVolume()), RANGE48DB, STEP150DB, 4);
		SetReg(0x80 + ch, (GetReg(0x80 + ch, 0) & 0xf0) | evol, 1);
	}
}

void COPKRhythm::UpdateVoice(uint8_t ch)
{
	CHATTR* attr = GetChAttribute(ch);
	if (attr) {
		uint8_t num = attr->GetVoice()->AL & 0xf;
		SetReg(0x80 + ch, (num << 4) | (GetReg(0x80 + ch, 0) & 0xf), 1);
	}
}

void COPKRhythm::UpdateKey(uint8_t ch, uint8_t keyon)
{
	CHATTR* attr = GetChAttribute(ch);
	if (attr) {
		uint8_t tmp = GetReg(0x84, 0) & ~(0x1 << ch);
		SetReg(0x84, tmp | ((keyon ? 1 : 0) << ch), 1);
	}
}

uint8_t COPKRhythm::QueryCh(CMidiCh* parent, FMVOICE* voice, int mode)
{
	return CSoundDevice::QueryCh(parent, voice, mode);
}
