#include "STDAFX.H"
#include "FITOM.h"
#include "OPK.h"

const UINT8 COPK::chofs[] = {0, 1, 4, 5, 8, 9, 12, 13, };

#define GET_AR(v,o)	(v->op[o].AR >> 3)
#define GET_DR(v,o)	(v->op[o].DR >> 3)
#define GET_SR(v,o)	(v->op[o].SR >> 3)
#define GET_RR(v,o)	(v->op[o].RR >> 3)
#define GET_SL(v,o)	(v->op[o].SL >> 3)
#define GET_TL(v,o)	(v->op[o].TL)
#define GET_RV(v,o)	(v->op[o].REV >> 3)

COPK::COPK(CPort* pt, int fsamp) : CSoundDevice(DEVICE_OPK, 8, fsamp, 108, FNUM_OFFSET, FnumTableType::Fnumber, pt)
{
	SetReg(0x01, 0xff, 1);
	SetReg(0x84, 0x00, 1);
	ops = 2;
	rhythmcap = 16;
	for (int i=0; i<4; i++) {
		RhythmStat[i] = 0xff;
	}
	NoteOffset = -69;
}

void COPK::UpdateVoice(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	SetReg(0x70 + ch, (GetReg(0x70 + ch, 0) & 0xf8) | (voice->FB & 0x7), 1);
	for (int i=0; i<2; i++) {
		UINT8 tmp;
		tmp = (GetReg(0x10 + chofs[ch] + (i * 2), 0) & 0x90) | voice->op[i].MUL;
		SetReg(0x10 + chofs[ch] + (i * 2), tmp | (voice->op[i].VIB ? 0x40 : 0) | (voice->op[i].WS ? 0x20 : 0), 1);
		tmp = GET_TL(voice, i);
		if (!i) {
			tmp = (tmp << 1) | (voice->NFQ ? 1: 0);
		}
		SetReg(0x20 + chofs[ch] + (i * 2), tmp);
		SetReg(0x30 + chofs[ch] + (i * 2), (GET_AR(voice, i) << 4) | GET_DR(voice, i));
		SetReg(0x40 + chofs[ch] + (i * 2), (GET_SL(voice, i) << 4) | GET_SR(voice, i));
	}
	UpdateVolExp(ch);
}

void COPK::UpdateVolExp(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	UINT8 evol = CalcLinearLevel(attr->GetEffectiveLevel(), voice->op[1].TL);
	attr->baseTL[1] = evol;
	evol = Linear2dB(evol, RANGE96DB, STEP075DB, 7);
	SetReg(0x22 + chofs[ch], evol & 0x7f);
}

void COPK::UpdateFreq(UINT8 ch, const FNUM* fnum)
{
	CHATTR* attr = GetChAttribute(ch);
	fnum = fnum ? fnum : attr->GetLastFnumber();
	UINT8 blk = fnum->block;
	UINT16 fn = fnum->fnum;
	if (fn & 0x800) {
		fn >>= 1;
		blk++;
	}
	SetReg(0x52 + chofs[ch], (blk << 5) | UINT8(fn >> 6), 1);
	SetReg(0x62 + chofs[ch], UINT8(fn << 2) & 0xf0, 1);
}

void COPK::UpdateTL(UINT8 ch, UINT8 op, UINT8 lev)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	if (op == 0) {
		SetReg(0x20 + chofs[ch], (lev << 1) | (voice->NFQ ? 1 : 0), 1);
	} else if (op == 1) {
		SetReg(0x22 + chofs[ch], (GetReg(0x22 + chofs[ch], 0) & 0x80) | lev, 1);
	}
}

void COPK::UpdateKey(UINT8 ch, UINT8 keyon)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	SetReg(0x40 + chofs[ch], (GET_SL(voice, 0) << 4) | (keyon ? GET_SR(voice, 0) : GET_RR(voice, 0)), 1);
	SetReg(0x42 + chofs[ch], (GET_SL(voice, 1) << 4) | (keyon ? GET_SR(voice, 1) : GET_RR(voice, 1)), 1);
	SetReg(0x12 + chofs[ch], (GetReg(0x12 + chofs[ch], 0) & 0x7f) | (keyon ? 0x80 : 0), 1);
}

void COPK::UpdateSustain(UINT8 ch)
{
}

void COPK::UpdatePanpot(UINT8 ch)
{
	int pan = (GetChAttribute(ch)->panpot) / 8;
	UINT8 chena = 0;
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

UINT8 COPK::GetRhythmCh(UINT8 num)
{
	UINT8 rch = 0xff;
	for (int i=0; i<4; i++) {
		if (RhythmStat[i] ==num) {
			SetReg(0x84, GetReg(0x84, 0) & ~(0x1 << i), 1);
			rch = i;
		}
	}
	if (rch == 0xff) {
		for (int i=0; i<4; i++) {
			if (RhythmStat[i] == -1) {
				RhythmStat[i] = num;
				rch = i;
			}
		}
	}
	if (rch == 0xff) {
		SetReg(0x84, GetReg(0x84, 0) & 0xfe, 1);
		RhythmStat[0] = num;
		rch = 0;
	}
	return rch;
}

void COPK::RhythmOn(UINT8 num, UINT8 vel, SINT8 pan, FMVOICE* rv, FNUM* fnum)
{
	//SetReg(0xbd, 0x20);
	if (num < rhythmcap) {
		UINT8 evol = Linear2dB(CalcLinearLevel(vel, 127-rhythmvol), RANGE48DB, STEP150DB, 4);
		UINT8 rch = GetRhythmCh(num);
		SetReg(0x80 + rch, (num << 4) | evol, 1);
		UINT8 tmp = GetReg(0x84, 0) & ~(0x1 << rch);
		SetReg(0x84, tmp | (0x1 << rch) , 1);
	}
}

void COPK::RhythmOff(UINT8 num)
{
	if (num < rhythmcap) {
		for (int i=0; i<4; i++) {
			if (RhythmStat[i] == num) {
				SetReg(0x84, GetReg(0x84, 0) & ~(0x1 << i), 1);
				RhythmStat[i] = 0xff;
			}
		}
	}
}

COPK2::COPK2(CPort* pt, int fsamp) : COPK(pt, fsamp)
{
	SetDevice(DEVICE_OPK2);
}
