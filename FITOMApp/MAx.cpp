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
	: CSoundDevice(DEVICE_SD1, 16, fsamp, 128, FNUM_OFFSET, FnumTableType::Fnumber, pt, 0x80), PresetIndex(0)
{

}

void CSD1::SetVoice(UINT8 ch, FMVOICE* voice, int update)
{
	CSoundDevice::SetVoice(ch, voice, update);
	int inst = -1;
	for (int i = 0; i < 16; i++) {
		if (memcmp(voice, &PresetTone[i], sizeof(FMVOICE)) == 0) {
			inst = i;
			break;
		}
	}
	if (inst == -1) {
	}
}

void CSD1::UpdatePresetTone()
{
}

void CSD1::UpdatePresetTone()
{
	BYTE tonebuf[1 + 1 + 480 + 3];
	int idx = 0;
	tonebuf[idx++] = 0x07;	//Register address
	tonebuf[idx++] = 0x90;	//Tone table size (always 16)
	tonebuf[idx++] = 0x07;	//Register address

}

void CSD1::UpdateVoice(UINT8 ch)
{
	UINT8 tmp;
	FMVOICE* voice = GetChAttribute(ch)->GetVoice();

	UINT8 inst = 0xff;
	for (int i = 0; i < 16; i++) {
		if (memcmp(voice, &PresetTone[i], sizeof(FMVOICE)) == 0) {
			inst = i;
			break;
		}
	}
	if (inst > 15) {
		return;
	}

	SetReg(0x0b, ch);
	SetReg(0x0f, inst & 0xf);
}

void CSD1::UpdateVolExp(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	UINT8 evol = CalcVolExpVel(127, attr->express, attr->velocity);
	evol = Linear2dB(evol, RANGE48DB, STEP075DB, 5);
	UINT8 cvol = Linear2dB(attr->volume, RANGE48DB, STEP075DB, 5);
	SetReg(0x0b, ch);
	SetReg(0x0d, evol << 2);
	SetReg(0x10, (cvol << 2) | 1);
}

void CSD1::UpdateFreq(UINT8 ch, const FNUM* fnum)
{
	CHATTR* attr = GetChAttribute(ch);
	fnum = fnum ? fnum : attr->GetLastFnumber();
	SetReg(0x0b, ch);
	SetReg(0x0d, ((fnum->fnum >> 5) & 0x78) | fnum->block);
	SetReg(0x0e, (fnum->fnum >> 1) & 0x7f);
}

void CSD1::UpdateTL(UINT8 ch, UINT8 op, UINT8 lev)
{
}

void CSD1::UpdateKey(UINT8 ch, UINT8 keyon)
{
	SetReg(0x0b, ch);
	SetReg(0x0f, (GetReg(0xf) & 0x3f) | (keyon ? 0x40 : 0));
}
