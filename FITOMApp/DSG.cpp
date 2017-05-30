#include "stdafx.h"
#include "DSG.h"
#include "MIDI.h"

//-------
CDSG::CDSG(CPort* pt, int fsamp) : CSoundDevice(DEVICE_DSG, 4, fsamp, 2, -576, FnumTableType::TonePeriod, pt, 0x20)
{
	SetReg(0x07, 0x3f, 1);
	ops = 1;
}

void CDSG::UpdateVolExp(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	UINT8 evol = Linear2dB(CalcLinearLevel(attr->GetEffectiveLevel(), 0), RANGE12DB, STEP600DB, 2);

	SetReg(0xc + ch, (GetReg(0xc + ch, 0) & 0xcf) | (evol << 4), 0);
}

void CDSG::UpdateFreq(UINT8 ch, const FNUM* fnum)
{
	fnum = fnum ? fnum : GetChAttribute(ch)->GetLastFnumber();
	UINT8 oct = (fnum->block < 4) ? fnum->block : 3;
	UINT16 etp = fnum->fnum >> ((fnum->block < 4) ? 0 : (fnum->block - 3));
	SetReg(0x0 + ch, etp & 0x7f, 0);
	SetReg(0x4 + ch, ((etp >> 7) | oct << 3 | (GetReg(0x4 + ch, 0) & 0x60)), 0);
}

void CDSG::UpdateVoice(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	UINT8 tmp = GetReg(0x8 + ch, 0) & 0x10;
	SetReg(0x8 + ch, tmp | ((voice->op[0].EGT & 0x3) << 5) | (voice->op[0].WS & 0x7), 1);
}

void CDSG::UpdateTL(UINT8 ch, UINT8 op, UINT8 lev)
{
	UINT8 tmp = GetReg(0xc + ch, 0) & 0x4f;
	SetReg(0xc + ch, tmp | ((lev >> 1) & 0x30), 0);
}

void CDSG::UpdatePanpot(UINT8 ch)
{
	SINT8 pan = GetChAttribute(ch)->panpot;
	UINT8 chena = 0;
	if (pan == 0) { //C
		chena = 3;
	}
	else if (pan > 0) { //R
		chena = 2;
	}
	else if (pan < 0) { //L
		chena = 1;
	}
	UINT8 tmp = GetReg(0xc + ch, 0) & 0x70;
	SetReg(0xc + ch, tmp | chena, 0);
}

void CDSG::UpdateKey(UINT8 ch, UINT8 keyon)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	CMidiCh* parent = attr->GetParent();
	if (keyon && parent && parent->GetForceDamp()) {
		SetReg(0x4 + ch, (GetReg(0x4 + ch, 0) & 0x1f) | 0x20, 1);
	}
	else {
		//UpdateSustain(ch);
	}
	SetReg(0x4 + ch, (keyon ? 0x40 : 0) | (GetReg(0x4 + ch, 0) & 0x1f), 1);
}

void CDSG::UpdateSustain(UINT8 ch)
{
	UINT8 sus = GetChAttribute(ch)->GetParent()->GetSustain() ? 0x10 : 0;
	UINT8 tmp = GetReg(0x8 + ch, 0) & 0x6f;
	SetReg(0x8 + ch, sus | tmp);
}

void CDSG::RhythmOn(UINT8 num, UINT8 vel, SINT8 pan, FMVOICE* rv, FNUM* fnum)
{
	//SetReg(0x0e, 0x20);
	if (num < rhythmcap) {
		RhythmOff(num);
		UINT8 evol = Linear2dB(CalcLinearLevel(rhythmvol, 127 - vel), RANGE24DB, STEP600DB, 2);
		SetReg(0x14 + num, evol, 1);
		SetReg(0x10, (GetReg(0x10, 0) & 0x1f) | (1 << num));
	}
}

void CDSG::RhythmOff(UINT8 num)
{
	if (num < rhythmcap) {
		//SetReg(0x10, (~(1 << num) & (GetReg(0x10, 0) & 0x1f)));
	}
}

void CDSG::SetReg(UINT16 reg, UINT8 data, UINT8 v)
{
	if (!v && regbak) {
		v = (regbak[reg] != data);
	}
	if (regbak) {
		regbak[reg] = data;
	}
	if (v && port) {
		port->writeRaw(0, reg | 0x80);
		port->writeRaw(0, data & 0x7f);
	}
}
