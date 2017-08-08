#include "stdafx.h"
#include "MAx.h"

#define GET_AR(v,o)	(v->op[o].AR >> 3)
#define GET_DR(v,o)	(v->op[o].DR >> 3)
#define GET_SR(v,o)	(v->op[o].SR >> 3)
#define GET_RR(v,o)	(v->op[o].RR >> 3)
#define GET_SL(v,o)	(v->op[o].SL >> 3)
#define GET_TL(v,o)	(v->op[o].TL)
//#define GET_RV(v,o)	(v->op[o].REV >> 3)
#define GET_RV(v,o)	(4)

CSD1::CSD1(CPort* pt, int fsamp)
	: CSoundDevice(DEVICE_SD1, 16, fsamp, 128, FNUM_OFFSET, FnumTableType::Fnumber, pt, 0x80)
{

}

void CSD1::SetPresetTone(UINT8 tn, FMVOICE* voice)
{
	if (tn < 16) {
		PresetTone[tn] = *voice;
	}
}

void CSD1::UpdatePresetTone()
{
	BYTE tonebuf[1 + 1 + 480 + 3];
	int idx = 0;
	tonebuf[idx++] = 0x07;	//Register address
	tonebuf[idx++] = 0x90;	//Tone table size
	tonebuf[idx++] = 0x07;	//Register address

}

void CSD1::UpdateVoice(UINT8 ch)
{
	UINT8 tmp;
	FMVOICE* voice = GetChAttribute(ch)->GetVoice();

	UINT8 inst = 0;
	if (voice->AL & 0x40) {//Preset Instrument
		inst = voice->AL & 0xf;
	}
	else {//User Instrument
		for (int i = 0; i<2; i++) {
			tmp = ((voice->op[i].AM & 0x1) << 7) | ((voice->op[i].VIB & 0x1) << 6) |
				(GET_SR(voice, i) ? 0 : 0x20) | ((voice->op[i].KSR & 0x1) << 4) |
				(voice->op[i].MUL & 0x0F);
			SetReg(0 + i, tmp);
			tmp = (GET_AR(voice, i) << 4) | GET_DR(voice, i);
			SetReg(4 + i, tmp);
			//tmp = (GET_SL(voice, i) << 4) | (GET_SR(voice, i) & 0xf);
			tmp = (GET_SL(voice, i) << 4) | ((IS_KEY(ch) ? GET_SR(voice, i) : GET_RR(voice, i)) & 0xf);
			SetReg(6 + i, tmp);
		}
		tmp = ((voice->op[0].KSL & 0x3) << 6) | GET_TL(voice, 0);
		SetReg(2, tmp);
		tmp = ((voice->op[1].KSL & 0x3) << 6) | ((voice->op[0].WS & 1) << 3) |
			((voice->op[1].WS & 1) << 4) | (voice->FB & 0x7);
		SetReg(3, tmp);
	}
	SetReg(0x30 + ch, (inst << 4) | (GetReg(0x30 + ch, 0) & 0x0f));
}

void CSD1::UpdateVolExp(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	UINT8 evol = CalcLinearLevel(attr->GetEffectiveLevel(), voice->op[1].TL);
	attr->baseTL[1] = evol;
	evol = Linear2dB(evol, RANGE48DB, STEP150DB, 4);
	SetReg(0x30 + ch, (GetReg(0x30 + ch, 0) & 0xf0) | evol, 0);
}

void CSD1::UpdateFreq(UINT8 ch, const FNUM* fnum)
{
	CHATTR* attr = GetChAttribute(ch);
	fnum = fnum ? fnum : attr->GetLastFnumber();
	SetReg(0x10 + ch, UINT8((fnum->fnum >> 2) & 0xff), 1);
	SetReg(0x20 + ch, (GetReg(0x20 + ch, 0) & 0x30) |
		(fnum->block << 1) | UINT8(fnum->fnum >> 10), 1);
}

void CSD1::UpdateSustain(UINT8 ch)
{
	UINT8 sus = GetChAttribute(ch)->GetParent()->GetSustain() ? 0x20 : 0;
	UINT8 tmp = GetReg(0x20 + ch, 0) & 0xdf;
	SetReg(0x20 + ch, sus | tmp);
}

void CSD1::UpdateTL(UINT8 ch, UINT8 op, UINT8 lev)
{
	lev = (lev > 63) ? 63 : lev & 0x3f;
	if (op == 0) {
		SetReg(0x2, (GetReg(2, 0) & 0xc0) | lev, 0);
	}
	else if (op == 1) {
		SetReg(0x30 + ch, (GetReg(0x30 + ch, 0) & 0xf0) | (lev >> 2), 0);
	}
}

void CSD1::UpdateKey(UINT8 ch, UINT8 keyon)
{
	UINT8 tmp;
	FMVOICE* voice = GetChAttribute(ch)->GetVoice();

	if (!(voice->AL & 0x40)) {//Preset Instrument
		for (int i = 0; i<2; i++) {
			tmp = (GET_SL(voice, i) << 4) | (((keyon && GET_SR(voice, i)) ? GET_SR(voice, i) : GET_RR(voice, i)) & 0xf);
			SetReg(0 + i, (GetReg(1, 0) & 0xdf) | ((keyon && GET_SR(voice, i)) ? 0 : 0x20));
			SetReg(6 + i, tmp);
		}
	}

	tmp = GetReg(0x20 + ch, 0) & 0xef;
	SetReg(0x20 + ch, tmp | (keyon ? 0x10 : 0));
}
