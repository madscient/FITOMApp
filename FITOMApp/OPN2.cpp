#include "STDAFX.h"
#include "OPN.h"

COPN2::COPN2(CPort* pt1, CPort* pt2, int fsamp, UINT8 devtype)
	: CSpanDevice(new COPN(pt1, fsamp, devtype), new COPN(pt2, fsamp, devtype))
{
	lfores = new LFORESOURCE[1];
	((COPN*)chips[1])->fxena = false;
}
COPN2::~COPN2()
{
	delete[] lfores;
}

void COPN2::Init()
{
	// 1/6, 1/4
	SetReg(0x2f, 0, 1);
	SetReg(0x2d, 0, 1);
	SetReg(0x27, 0x0, 1);
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
//	COPN::COPN(pt1, fsamp, devtype);
}

void COPNA::Init()
{
	// 1/6, 1/4
	SetReg(0x2f, 0, 1);
	SetReg(0x2d, 0, 1);
	SetReg(0x27, 0x0, 1);

	SetReg(0x29, 0x80, 1);
	SetReg(0x10, 0xbf);
	SetReg(0x10, 0);
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
}

void COPN3L::Init()
{
	COPNA::Init();
	SetReg(0x20, 0x02, 1);
}

//-----
COPNARhythm::COPNARhythm(CSoundDevice* pParent) : CRhythmDevice(pParent, DEVICE_OPNA_RHY, 6)
{
}

void COPNARhythm::UpdateVolExp(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	if (attr) {
		UINT8 evol = 31 - Linear2dB(CalcLinearLevel(attr->velocity, 0), RANGE24DB, STEP075DB, 5);
		SetReg(0x18 + ch, (GetReg(0x18, 0) & 0xc0) | evol, 1);	//Instrumental Level

		evol = 63 - Linear2dB(CalcLinearLevel(attr->volume, 0), RANGE48DB, STEP075DB, 6);
		if (GetReg(0x11, 0) != evol) {
			SetReg(0x11, evol);
		}
	}
}

void COPNARhythm::UpdatePanpot(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	if (attr) {
		SINT8 pan = attr->panpot / 8;
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
		SetReg(0x18 + ch, chena | (GetReg(0x18, 0) & 0x3f), 1);	//Instrumental Level
	}
}

void COPNARhythm::UpdateKey(UINT8 ch, UINT8 keyon)
{
	CHATTR* attr = GetChAttribute(ch);
	if (attr) {
		SetReg(0x10, (1 << ch), 1);
	}
}

UINT8 COPNARhythm::QueryCh(CMidiCh* parent, FMVOICE* voice, int mode)
{
	UINT8 ret = 0xff;
	if (voice) {
		ret = voice->AL & 0xf;
	}
	return ret;
}
