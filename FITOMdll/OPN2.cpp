#include "STDAFX.h"
#include "FITOM.h"
#include "OPN.h"
#include "MIDI.h"

COPN2::COPN2(CPort* pt1, CPort* pt2, int fsamp, UINT8 devtype)
	: CSpanDevice(new COPN(pt1, fsamp, devtype), new COPN(pt2, fsamp, devtype))
{
	lfores = new LFORESOURCE[1];
}
COPN2::~COPN2()
{
	delete[] lfores;
}

void COPN2::UpdateVoice(UINT8 ch)
{
	UINT8 tmp;
	CSpanDevice::UpdateVoice(ch);
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	tmp = 0xc0 | (voice->AMS << 4) | voice->PMS;
	chips[chres[ch].dev]->SetReg(0xb4 + (chres[ch].ch), tmp&0xc0);
}

void COPN2::UpdateKey(UINT8 ch, UINT8 keyon)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	CMidiCh* parent = attr->GetParent();
	if (keyon && parent && parent->GetForceDamp()) {
		for (int i=0; i<4; i++) {
			UINT8 tmp = (voice->op[i].SL << 4) | 0xf;
			chips[chres[ch].dev]->SetReg(0x80 + COPN::map[i] + chres[ch].ch, tmp, 1);
		}
	} else {
		UpdateSustain(ch);
	}
	SetReg(0x28, (keyon ? 0xf0 : 0) | (ch%3) | ((ch>2) ? 4 : 0), 1);
}

void COPN2::EnableDevPM(UINT8 ch, UINT8 on)
{
	CHATTR* attr = GetChAttribute(ch);
	CMidiCh* parent = attr->GetParent();
	if (ch < chs && on) {
		if (lfores[0].parent != parent) {
			lfores[0].used = 0L;
		}
		lfores[0].parent = parent;
		lfores[0].used |= (1 << ch);
	} else if (ch < chs) {
		lfores[0].used &= ~UINT32(1 << ch);
		if (lfores[0].used == 0L) {
			lfores[0].parent = 0;
		}
	}
}

void COPN2::EnableDevAM(UINT8 ch, UINT8 on)
{
	CHATTR* attr = GetChAttribute(ch);
	CMidiCh* parent = attr->GetParent();
	if (ch < chs && on) {
		if (lfores[0].parent != parent) {
			lfores[0].used = 0L;
		}
		lfores[0].parent = parent;
		lfores[0].used |= (1 << ch);
	} else if (ch < chs) {
		lfores[0].used &= ~UINT32(1 << ch);
		if (lfores[0].used == 0L) {
			lfores[0].parent = 0;
		}
	}
}

void COPN2::SetDevPMDepth(UINT8 ch, UINT8 dep)
{
	CHATTR* attr = GetChAttribute(ch);
	CMidiCh* parent = attr->GetParent();
	if (ch < chs) {
		if (lfores[0].used && lfores[0].parent != parent) {
			EnableDevPM(ch, dep);
		}
		if (lfores[0].pmdepth != dep) {
			lfores[0].pmdepth = dep;
			UINT8 op = chres[ch].dev;
			UINT8 dch = chres[ch].ch;
			chips[op]->SetReg(0xb4 + dch, (chips[op]->GetReg(0xb4 + dch, 0) & 0xf8) | (dep >> 4));
		}
	}
}

void COPN2::SetDevAMDepth(UINT8 ch, UINT8 dep)
{
	CHATTR* attr = GetChAttribute(ch);
	CMidiCh* parent = attr->GetParent();
	if (ch < chs) {
		if (lfores[0].used && lfores[0].parent != parent) {
			EnableDevAM(ch, dep);
		}
		if (lfores[0].amdepth != dep) {
			lfores[0].amdepth = dep;
			UINT8 op = chres[ch].dev;
			UINT8 dch = chres[ch].ch;
			chips[op]->SetReg(0xb4 + dch, (chips[op]->GetReg(0xb4 + dch, 0) & 0xc7) | ((dep >> 5) << 4));
		}
	}
}

void COPN2::SetDevAMRate(UINT8 ch, UINT8 rate)
{
	if (lfores[0].amrate != rate) {
		lfores[0].amrate = rate;
		SetReg(0x22, rate >> 3);
	}
}

void COPN2::SetDevPMRate(UINT8 ch, UINT8 rate)
{
	if (lfores[0].pmrate != rate) {
		lfores[0].pmrate = rate;
		SetReg(0x22, rate >> 3);
	}
}
/*----------------*/
COPN2C::COPN2C(CPort* pt1, CPort* pt2, int fsamp) : COPN2(pt1, pt2, fsamp, DEVICE_OPN2C)
{
}

/*----------------*/
COPN2L::COPN2L(CPort* pt1, CPort* pt2, int fsamp) : COPN2(pt1, pt2, fsamp, DEVICE_OPN2L)
{
}

/*----------------*/

COPNA::COPNA(CPort* pt1, CPort* pt2, int fsamp, UINT8 devtype) : COPN2(pt1, pt2, fsamp, devtype)
{
	COPN::COPN(pt1, fsamp, devtype);
	SetReg(0x29, 0x80, 1);
	rhythmcap = 6;
}

void COPNA::UpdateRhythmVol()
{
	UINT8 evol = 63 - (CalcEffectiveLevel(rhythmvol, 0) >> 1);
	if (GetReg(0x11, 0) != evol) {
		SetReg(0x11, evol);
	}
}

void COPNA::RhythmOn(UINT8 num, UINT8 vel, SINT8 pan, FMVOICE* rv, FNUM* fnum)
{
	UINT8 evol = 31 - (CalcEffectiveLevel(vel, 0) >> 2);
	if (num < rhythmcap) {
		SetReg(0x18 + num, 0xc0 | evol, 1);	//Instrumental Level
		SetReg(0x10, (1 << num), 1);
	}
}

/*----------------*/
COPNB::COPNB(CPort* pt1, CPort* pt2, int fsamp, UINT8 devtype) : COPN2(pt1, pt2, fsamp, devtype)
{
	EnableCh(0, 0);
	EnableCh(3, 0);
}

/*----------------*/
CF286::CF286(CPort* pt1, CPort* pt2, int fsamp) : COPNB(pt1, pt2, fsamp, DEVICE_F286)
{
}

/*----------------*/
C2610B::C2610B(CPort* pt1, CPort* pt2, int fsamp) : COPNB(pt1, pt2, fsamp, DEVICE_2610B)
{
	EnableCh(0, 1);
	EnableCh(3, 1);
}

/*----------------*/
COPN3L::COPN3L(CPort* pt1, CPort* pt2, int fsamp) : COPNA(pt1, pt2, fsamp, DEVICE_OPN3L)
{
	SetReg(0x20, 0x02, 1);
}
