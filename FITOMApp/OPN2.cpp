#include "STDAFX.h"
#include "OPN.h"

COPN2::COPN2(CPort* pt1, CPort* pt2, int fsamp, uint8_t devtype)
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

void COPN2::UpdateVoice(uint8_t ch)
{
	uint8_t tmp;
	CSpanDevice::UpdateVoice(ch);
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	tmp = 0xc0 | (voice->AMS << 4) | voice->PMS;
	chips[chres[ch].dev]->SetReg(0xb4 + (chres[ch].ch), tmp&0xc0);
}

void COPN2::UpdateKey(uint8_t ch, uint8_t keyon)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	CMidiCh* parent = attr->GetParent();
	if (keyon && parent && parent->GetForceDamp()) {
		for (int i=0; i<4; i++) {
			uint8_t tmp = (voice->op[i].SL << 4) | 0xf;
			chips[chres[ch].dev]->SetReg(0x80 + COPN::map[i] + chres[ch].ch, tmp, 1);
		}
	} else {
		UpdateSustain(ch);
	}
	SetReg(0x28, (keyon ? 0xf0 : 0) | (ch%3) | ((ch>2) ? 4 : 0), 1);
}

void COPN2::EnableDevPM(uint8_t ch, uint8_t on)
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
		lfores[0].used &= ~uint32_t(1 << ch);
		if (lfores[0].used == 0L) {
			lfores[0].parent = 0;
		}
	}
}

void COPN2::EnableDevAM(uint8_t ch, uint8_t on)
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
		lfores[0].used &= ~uint32_t(1 << ch);
		if (lfores[0].used == 0L) {
			lfores[0].parent = 0;
		}
	}
}

void COPN2::SetDevPMDepth(uint8_t ch, uint8_t dep)
{
	CHATTR* attr = GetChAttribute(ch);
	CMidiCh* parent = attr->GetParent();
	if (ch < chs) {
		if (lfores[0].used && lfores[0].parent != parent) {
			EnableDevPM(ch, dep);
		}
		if (lfores[0].pmdepth != dep) {
			lfores[0].pmdepth = dep;
			uint8_t op = chres[ch].dev;
			uint8_t dch = chres[ch].ch;
			chips[op]->SetReg(0xb4 + dch, (chips[op]->GetReg(0xb4 + dch, 0) & 0xf8) | (dep >> 4));
		}
	}
}

void COPN2::SetDevAMDepth(uint8_t ch, uint8_t dep)
{
	CHATTR* attr = GetChAttribute(ch);
	CMidiCh* parent = attr->GetParent();
	if (ch < chs) {
		if (lfores[0].used && lfores[0].parent != parent) {
			EnableDevAM(ch, dep);
		}
		if (lfores[0].amdepth != dep) {
			lfores[0].amdepth = dep;
			uint8_t op = chres[ch].dev;
			uint8_t dch = chres[ch].ch;
			chips[op]->SetReg(0xb4 + dch, (chips[op]->GetReg(0xb4 + dch, 0) & 0xc7) | ((dep >> 5) << 4));
		}
	}
}

void COPN2::SetDevAMRate(uint8_t ch, uint8_t rate)
{
	if (lfores[0].amrate != rate) {
		lfores[0].amrate = rate;
		SetReg(0x22, rate >> 3);
	}
}

void COPN2::SetDevPMRate(uint8_t ch, uint8_t rate)
{
	if (lfores[0].pmrate != rate) {
		lfores[0].pmrate = rate;
		SetReg(0x22, rate >> 3);
	}
}

/*----------------*/

COPNA::COPNA(CPort* pt1, CPort* pt2, int fsamp, uint8_t devtype) : COPN2(pt1, pt2, fsamp, devtype)
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
COPNB::COPNB(CPort* pt1, CPort* pt2, int fsamp, uint8_t devtype) : COPN2(pt1, pt2, fsamp, devtype)
{
}

void COPNB::Init()
{
	EnableCh(0, 0);
	EnableCh(3, 0);
}

/*----------------*/
void COPN3L::Init()
{
	COPNA::Init();
	SetReg(0x20, 0x02, 1);
}

//-----
COPNARhythm::COPNARhythm(COPNA* pParent) : CRhythmDevice(pParent, DEVICE_OPNA_RHY, 6)
{
}

void COPNARhythm::UpdateVolExp(uint8_t ch)
{
	CHATTR* attr = GetChAttribute(ch);
	if (attr) {
		int8_t pan = attr->GetPanpot() / 8;
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
		uint8_t evol = 31 - Linear2dB(CalcLinearLevel(attr->velocity, 0), RANGE24DB, STEP075DB, 5);
		SetReg(0x18 + ch, chena | evol, 1);	//Instrumental Level

		evol = 63 - Linear2dB(CalcLinearLevel(attr->GetVolume(), 0), RANGE48DB, STEP075DB, 6);
		if (GetReg(0x11, 0) != evol) {
			SetReg(0x11, evol);
		}
	}
}

void COPNARhythm::UpdatePanpot(uint8_t ch)
{
	UpdateVolExp(ch);
}

void COPNARhythm::UpdateKey(uint8_t ch, uint8_t keyon)
{
	CHATTR* attr = GetChAttribute(ch);
	if (attr && keyon) {
		SetReg(0x10, (1 << ch), 1);
	}
}

uint8_t COPNARhythm::QueryCh(CMidiCh* parent, FMVOICE* voice, int mode)
{
	uint8_t ret = 0xff;
	if (voice) {
		ret = voice->AL & 0xf;
	}
	return ret;
}

COPN3LRhythm::COPN3LRhythm(COPN3L* pParent) : COPNARhythm(pParent)
{
	device = DEVICE_OPN3L_RHY;
}
