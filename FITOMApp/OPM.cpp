#include "STDAFX.h"
#include "OPM.h"

uint8_t COPM::KeyCode[] = { 0, 1, 2, 4, 5, 6, 8, 9, 10, 12, 13, 14, };
uint8_t COPM::map[] = {0, 2, 1, 3};
uint8_t COPM::carmsk[] = { 0x8, 0x8, 0x8, 0x8, 0xa, 0xe, 0xe, 0xf, };

#define GET_AR(v,o)	(v->op[o].AR >> 2)
#define GET_DR(v,o)	(v->op[o].DR >> 2)
#define GET_SR(v,o)	(v->op[o].SR >> 2)
#define GET_RR(v,o)	(v->op[o].RR >> 3)
#define GET_SL(v,o)	(v->op[o].SL >> 3)
#define GET_TL(v,o)	(v->op[o].TL)
#define GET_RV(v,o)	(v->op[o].REV)

COPM::COPM(CPort* pt, int fsamp) : CSoundDevice(DEVICE_OPM, 8, 0, 0, 0, FnumTableType::none, pt, 0x100), lfos(1)
{
	//if (pt) { pt->reset(); }
	lfores = new LFORESOURCE[1];
	MasterTune = round(theFnum.GetOPMMasterOffset(fsamp));
	NoteOffset = -61;	// origin note: O4C+
}

void COPM::Init()
{
	SetReg(0x01, 0x00, 1);
	SetReg(0x14, 0x00, 1);
}

ISoundDevice::FNUM COPM::GetFnumber(uint8_t ch, int16_t offset)
{
	CHATTR* attr = GetChAttribute(ch);
	FNUM ret;
	int oct = (-NoteOffset / 12) - 1;
	int index = attr->GetNoteIndex(MasterTune + (NoteOffset * 64) + offset);

	// normalize
	while (index < 0) {
		oct--;
		index += 768;
	}
	while (index >= 768) {
		oct++;
		index -= 768;
	}
	if (oct >= 0 && oct < 8) {
		//ret.block = (oct << 4) | COPM::KeyCode[index >> 6];
		//ret.fnum = (attr->GetLastNote() << 8) | (uint8_t(index & 0x3f) << 2);	//Key Fraction
		ret.block = oct;
		ret.fnum = (uint16_t(COPM::KeyCode[index >> 6]) << 6) | (uint8_t(index & 0x3f));
	} else {
		ret.block = 0;
		ret.fnum = 0;
	}
	return ret;
}

void COPM::UpdateVoice(uint8_t ch)
{
	uint8_t	tmp;
	uint8_t	i;
	FMVOICE* voice = GetChAttribute(ch)->GetVoice();

	tmp = (GetReg(0x20 + ch, 0) & 0xc0) | (voice->FB << 3) | (voice->AL & 7);
	SetReg(0x20 + ch, tmp);
	SetReg(0x38 + ch, 0);	//LFO disable

	for (i=0; i<4; i++)
	{
		uint8_t ofm = voice->op[map[i]].DM0;
		tmp = ((voice->op[map[i]].DT1 & 0x7) << 4) | (voice->op[map[i]].MUL & 0xf);
		SetReg(0x40 + i * 8 + ch, tmp);
		SetReg(0x60 + i * 8 + ch, GET_TL(voice, map[i]));
		tmp = ((voice->op[map[i]].KSR & 0x3) << 6) | GET_AR(voice, map[i]) | (ofm ? 0x20 : 0);
		SetReg(0x80 + i * 8 + ch, tmp);
		tmp = ((voice->op[map[i]].AM & 0x1) << 7) | GET_DR(voice, map[i]);
		SetReg(0xa0 + i * 8 + ch, tmp);
		tmp = (ofm ? 0 : ((voice->op[map[i]].DT2 & 0x3) << 6)) | GET_SR(voice, map[i]);
		SetReg(0xc0 + i * 8 + ch, tmp);
		tmp = (GET_SL(voice, map[i]) << 4) | GET_RR(voice, map[i]);
		SetReg(0xe0 + i * 8 + ch, tmp);
	}
	if (ch == 7) {
		if (voice->AL & 0x8) {
			SetReg(0xf, 0x80 | voice->NFQ);
		} else {
			SetReg(0xf, 0);
		}
	}
	UpdatePanpot(ch);
	UpdateVolExp(ch);
}

void COPM::UpdateVolExp(uint8_t ch)
{
	CHATTR* attr = GetChAttribute(ch);
	uint8_t evol = attr->GetEffectiveLevel();
	FMVOICE* voice = attr->GetVoice();
	for (int i=0; i<4; i++) {
		if (carmsk[voice->AL&7] & (1<<i)) {
			uint8_t tl = CalcLinearLevel(evol, voice->op[i].TL);
			attr->baseTL[i] = tl;
			tl = Linear2dB(tl, RANGE96DB, STEP075DB, 7);
			SetReg(0x60 + map[i] * 8 + ch, tl, 0);
		}
	}
}

void COPM::UpdateFreq(uint8_t ch, const FNUM* fnum)
{
	fnum = fnum ? fnum : GetChAttribute(ch)->GetLastFnumber();
	uint8_t kc = (fnum->block << 4) | (fnum->fnum >> 6);
	uint8_t kf = fnum->fnum << 2;
	SetReg(0x28 + ch, kc, 0);	// kc
	SetReg(0x30 + ch, kf | (GetReg(0x30 + ch, 0) & 0x3), 0);	// kf
}

void COPM::UpdatePanpot(uint8_t ch)
{
	int pan = (GetChAttribute(ch)->GetPanpot()) / 8;
	uint8_t chena = 0;
	if (pan >= 4) { //R
		chena = 0x80;
	}
	else if (pan <= -4) { //L
		chena = 0x40;
	}
	else {	//C
		chena = 0xc0;
	}
	if ((GetReg(0x20 + ch, 0) & 0xc0) != chena) {
		SetReg(0x20 + ch, (GetReg(0x20 + ch, 0) & 0x3f) | chena, 1);
	}
}

void COPM::UpdateSustain(uint8_t ch)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	CMidiCh* parent = attr->GetParent();
	for (int i=0; i<4; i++) {
		uint8_t rr = ((parent && parent->GetSustain()) ? 4 : GET_RR(voice, map[i])) & 0xf;
		uint8_t tmp = (GET_SL(voice, map[i]) << 4) | rr;
		SetReg(0xe0 + (i * 8) + ch, tmp);
	}
}

void COPM::UpdateTL(uint8_t ch, uint8_t op, uint8_t lev)
{
	SetReg(0x60 + map[op] * 8 + ch, lev);
}

void COPM::UpdateKey(uint8_t ch, uint8_t keyon)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	CMidiCh* parent = attr->GetParent();
	if (keyon && parent && parent->GetForceDamp()) {
		for (int i=0; i<4; i++) {
			uint8_t tmp = (GET_SL(voice, map[i]) << 4) | 0xf;
			SetReg(0xe0 + (i * 8) + ch, tmp);
		}
	}
	if (!keyon) {
		UpdateSustain(ch);
	}
	SetReg(0x08, (keyon ? 0x78 : 0) | ch, 1);
}

uint8_t COPM::QueryCh(CMidiCh* parent, FMVOICE* voice, int mode)
{
	CHATTR* attr7 = GetChAttribute(7);
	if (voice && (voice->AL & 0x8)) {//Noise enabled
		if (mode ? attr7->IsAvailable() : attr7->IsEnable()) {
			return 7;
		} else {
			return 0xff;
		}
	}
	return CSoundDevice::QueryCh(parent, voice, mode);
}

void COPM::EnableDevPM(uint8_t ch, uint8_t on)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	CMidiCh* parent = attr->GetParent();
	if (ch < chs && on) {
		if (lfores[0].parent != parent) {
			for (int i=0; i<chs; i++) {
				if (lfores[0].used & (1 << i)) {
					//SetReg(0xa0 + i, GetReg(0xa0 + i, 0) & 0x7f);
					//SetReg(0xa8 + i, GetReg(0xa8 + i, 0) & 0x7f);
					//SetReg(0xb0 + i, GetReg(0xb0 + i, 0) & 0x7f);
					//SetReg(0xb8 + i, GetReg(0xb8 + i, 0) & 0x7f);
					SetReg(0x38+i, GetReg(0x38+i, 0) & 3);
				}
			}
			lfores[0].used = 0L;
		}
		lfores[0].parent = parent;
		lfores[0].used |= (1 << ch);
		SetReg(0x38 + ch, (GetReg(0x38 + ch, 0) & 0x3) | (voice->PMS << 4));
	} else if (ch < chs) {
		SetReg(0x38 + ch, (GetReg(0x38 + ch, 0) & 0x3));
		lfores[0].used &= ~uint32_t(1 << ch);
		if (lfores[0].used == 0L) {
			lfores[0].parent = 0;
		}
	}
}

void COPM::EnableDevAM(uint8_t ch, uint8_t on)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	CMidiCh* parent = attr->GetParent();
	if (ch < chs && on) {
		if (lfores[0].parent != parent) {
			for (int i=0; i<chs; i++) {
				if (lfores[0].used & (1 << i)) {
					SetReg(0x38+i, GetReg(0x38+i, 0) & 0x70);
				}
			}
			lfores[0].used = 0L;
		}
		lfores[0].parent = parent;
		lfores[0].used |= (1 << ch);
		SetReg(0x38 + ch, (GetReg(0x38 + ch, 0) & 0x70) | (voice->AMS));
	} else if (ch < chs) {
		SetReg(0x38 + ch, (GetReg(0x38 + ch, 0) & 0x70));
		lfores[0].used &= ~uint32_t(1 << ch);
		if (lfores[0].used == 0L) {
			lfores[0].parent = 0;
		}
	}
}

void COPM::SetDevPMDepth(uint8_t ch, uint8_t dep)
{
	if (ch < chs) {
		if (lfores[0].used && lfores[0].parent != GetChAttribute(ch)->GetParent()) {
			EnableDevPM(ch, dep);
		}
		if (lfores[0].pmdepth != dep) {
			lfores[0].pmdepth = dep;
			SetReg(0x19, 0x80 | dep);
		}
	}
}

void COPM::SetDevAMDepth(uint8_t ch, uint8_t dep)
{
	if (ch < chs) {
		if (lfores[0].used && lfores[0].parent != GetChAttribute(ch)->GetParent()) {
			EnableDevAM(ch, dep);
		}
		if (lfores[0].amdepth != dep) {
			lfores[0].amdepth = dep;
			SetReg(0x19, dep & 0x7f);
		}
	}
}

void COPM::SetDevAMRate(uint8_t ch, uint8_t rate)
{
	if (lfores[0].amrate != rate) {
		lfores[0].amrate = rate;
		SetReg(0x18, rate << 1);
	}
}

void COPM::SetDevPMRate(uint8_t ch, uint8_t rate)
{
	if (lfores[0].pmrate != rate) {
		lfores[0].pmrate = rate;
		SetReg(0x18, rate << 1);
	}
}

//-----

COPP::COPP(CPort* pt, int fsamp) : COPM(pt, fsamp)
{
	SetDevice(DEVICE_OPP);
}

void COPP::Init()
{
	SetReg(0x00, 0x10, 1);
	SetReg(0x01, 0x0, 1);
	SetReg(0x02, 0x10, 1);
	SetReg(0x03, 0x10, 1);
	SetReg(0x04, 0x10, 1);
	SetReg(0x05, 0x10, 1);
	SetReg(0x06, 0x10, 1);
	SetReg(0x07, 0x10, 1);
}

//-----
COPZ::COPZ(CPort* pt, int fsamp) : COPM(pt, fsamp)
{
	SetDevice(DEVICE_OPZ);
	delete[] lfores;
	lfores = new LFORESOURCE[2];
}

void COPZ::Init()
{
	SetReg(0x00, 0x10, 1);
	SetReg(0x01, 0x10, 1);
	SetReg(0x02, 0x10, 1);
	SetReg(0x03, 0x10, 1);
	SetReg(0x04, 0x10, 1);
	SetReg(0x05, 0x10, 1);
	SetReg(0x06, 0x10, 1);
	SetReg(0x07, 0x10, 1);
	SetReg(0x09, 0x0, 1);
	SetReg(0x0a, 0x4, 1);
	SetReg(0x0f, 0x0, 1);
	SetReg(0x14, 0x70, 1);
	SetReg(0x15, 0x01, 1);
	SetReg(0x1c, 0x0, 1);
	SetReg(0x1e, 0x0, 1);
}

void COPZ::UpdatePanpot(uint8_t ch)
{
	int pan = (GetChAttribute(ch)->GetPanpot()) / 8;
	uint8_t chena = 0;
	uint8_t mono = 0;
	if (pan >= 4) { //R
		chena = 0x80;
	}
	else if (pan <= -4) { //L
		chena = 0x40;
	}
	else { //C
		chena = 0xc0;
		mono = 0x1;
	}
	SetReg(0x20 + ch, (GetReg(0x20 + ch, 0) & 0x3f) | chena, 0);
	SetReg(0x30 + ch, (GetReg(0x30 + ch, 0) & 0xfe) | mono, 0);
}

void COPZ::UpdateVoice(uint8_t ch)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	for (int i = 0; i < 4; i++) {
		uint8_t tmp;
		uint8_t ofm = voice->op[map[i]].DM0;
		tmp = 0x80 | ((voice->op[map[i]].WS & 0x7) << 4) | (voice->op[map[i]].DT3 & 0xf);
		SetReg(0x40 + i * 8 + ch, tmp);
		//tmp = ((voice->op[map[i]].EGS & 0x3) << 6) | GET_RV(voice, map[i]) | 0x20;
		tmp = 0x20;
		SetReg(0xc0 + i * 8 + ch, tmp);
	}
	COPM::UpdateVoice(ch);
}
