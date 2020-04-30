#include "STDAFX.h"
#include "OPL.h"


uint8_t COPL::map[] = { 0, 1, 2, 8, 9, 10, 16, 17, 18, };

#if 0
uint8_t COPL::RhythmReg[] = { 0x51, 0x55, 0x52, 0x54, 0x53, };
uint8_t COPL::RhythmMapCh[] = { 7, 8, 8, 7, 6, };
ISoundDevice::FNUM COPL::RhythmFnum[] = {	
	ISoundDevice::FNUM(2, 0x480), ISoundDevice::FNUM(2, 0x540), ISoundDevice::FNUM(0, 0x700)
};
uint8_t COPL::RhythmFreq[] = { 47, 60, 53, };
#endif

//OPL Voice macro
#define GET_AR(v,o)	(v->op[o].AR >> 3)
#define GET_DR(v,o)	(v->op[o].DR >> 3)
#define GET_SR(v,o)	(v->op[o].SR >> 3)
#define GET_RR(v,o)	(v->op[o].RR >> 3)
#define GET_SL(v,o)	(v->op[o].SL >> 3)
#define GET_TL(v,o)	((v->op[o].TL) < 64 ? (v->op[o].TL) :63)
#define GET_AM(v,o)	(v->op[o].AVF & 1)
#define GET_VIB(v,o) ((v->op[o].AVF >> 1) & 1)
#define GET_FIX(v,o) ((v->op[o].AVF >> 2) & 1)
#define GET_KSL(v,o) ((v->op[o].KS >> 4) & 3)
#define GET_KSR(v,o) (v->op[o].KS & 1)
//#define GET_RV(v,o)	(v->op[o].REV >> 3)
#define GET_RV(v,o)	(4)


COPL::COPL(CPort* pt, int fsamp, uint8_t devtype)
	: CSoundDevice(devtype, 9, fsamp, 72, FNUM_OFFSET, FnumTableType::Fnumber, pt, 0x100)
{
	ops = 2;
}

void COPL::Init()
{
	SetReg(0x04, 0, 1);
	SetReg(0x08, 0, 1);
}

void COPL::UpdateVoice(uint8_t ch)
{
	uint8_t tmp;
	uint8_t i = 0;
	uint8_t ex = 0;
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	CMidiCh* parent = attr->GetParent();

	for(i=0; i<2; i++)
	{
		tmp = (GET_AM(voice, i) << 7) | (GET_VIB(voice, i) << 6) |
			(GET_SR(voice, i)?0:0x20) | (GET_KSR(voice, i) << 4) |
			(voice->op[i].MUL & 0x0F);
		SetReg(0x20 + i * 3 + map[ch], tmp);

		tmp = (uint8_t)((GET_KSL(voice, i) << 6) | GET_TL(voice, i));
		SetReg(0x40 + i * 3 + map[ch], tmp);

		tmp = (GET_AR(voice, i) << 4) | GET_DR(voice, i);
		SetReg(0x60 + i * 3 + map[ch], tmp);

		uint8_t rr = ((parent && parent->GetSustain() && ((i==1) || (voice->AL&1))) ?
			GET_RV(voice, i) : (GET_SR(voice, i) ? GET_SR(voice, i) : GET_RR(voice, i))) & 0xf;
		tmp = ((voice->op[i].SL & 0xf) << 4) | rr;
		SetReg(0x80 + i * 3 + map[ch], tmp);
	}
	tmp = 0xf0 | ((voice->FB & 0x7) << 1) | (voice->AL & 0x1);
	SetReg(0xc0 + ch, tmp);

	SetReg(0xe0 + map[ch], voice->op[0].WS);
	SetReg(0xe3 + map[ch], voice->op[1].WS);
	/*
	if (!(voice->AL & 0x4) || (voice->ID>>28)==2) {
		UpdateVolExp(ch);
	}
	*/
	UpdateVolExp(ch);
}

void COPL::UpdateVolExp(uint8_t ch)
{
	uint8_t tl;
	uint8_t tmp;
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	CMidiCh* parent = attr->GetParent();

	uint8_t evol = attr->GetEffectiveLevel();
	if (voice->AL & 1) {
		tl = CalcLinearLevel(evol, voice->op[0].TL);
		attr->baseTL[0] = tl;
		tl = Linear2dB(tl, RANGE48DB, STEP075DB, 6);
		tmp = (uint8_t)((GET_KSL(voice, 0) << 6) | tl);
		SetReg(0x40 + map[ch], tmp, 0);
	}
	tl = CalcLinearLevel(evol, voice->op[1].TL);
	attr->baseTL[1] = tl;
	tl = Linear2dB(tl, RANGE48DB, STEP075DB, 6);
	tmp = (uint8_t)((GET_KSL(voice, 1) << 6) | tl);
	SetReg(0x43 + map[ch], tmp, 0);
}

void COPL::UpdateFreq(uint8_t ch, const FNUM* fnum)
{
	fnum = fnum ? fnum : GetChAttribute(ch)->GetLastFnumber();
	SetReg(0xa0 + ch, (uint8_t)((fnum->fnum>>1) & 0xff), 1);
	SetReg(0xb0 + ch, (GetReg(0xb0 + ch, 0) & 0x20) |
		(uint8_t)((fnum->block << 2) | (fnum->fnum >> 9)), 1);
}

void COPL::UpdatePanpot(uint8_t ch)
{
	int pan = (GetChAttribute(ch)->GetPanpot()) / 8;
	uint8_t chena = 0;
	if (pan >= 4) { //R
		chena = 0x10;
	}
	else if (pan <= -4) { //L
		chena = 0x20;
	}
	else {	//C
		chena = 0x30;
	}
	if ((GetReg(0xc0 + ch, 0) & 0xf0) != chena) {
		SetReg(0xc0 + ch, (GetReg(0xc0 + ch, 0) & 0x0f) | chena);
	}
}

void COPL::UpdateSustain(uint8_t ch)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	CMidiCh* parent = attr->GetParent();
	if (voice->AL & 1) {
		uint8_t rr = (parent && parent->GetSustain()) ? GET_RV(voice, 0) : (GET_SR(voice, 0) ? GET_SR(voice, 0) : GET_RR(voice, 0));
		uint8_t tmp = (GET_SL(voice, 0) << 4) | (rr & 0xf);
		SetReg(0x80 + map[ch], tmp);
	}
	uint8_t rr = (parent && parent->GetSustain()) ? GET_RV(voice, 1) : (GET_SR(voice, 1) ? GET_SR(voice, 1) : GET_RR(voice, 1));
	uint8_t tmp = (GET_SL(voice, 1) << 4) | (rr & 0xf);
	SetReg(0x83 + map[ch], tmp);
}

void COPL::UpdateTL(uint8_t ch, uint8_t op, uint8_t lev)
{
	lev = (lev > 63) ? 63 : (lev & 63);
	SetReg(0x40 + (3 * op) + map[ch], lev, 0);
}


void COPL::UpdateKey(uint8_t ch, uint8_t keyon)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	CMidiCh* parent = attr->GetParent();
	uint8_t tmp;
	for (int i=0; i<2; i++) {
		tmp = (GetReg(0x20 + i * 3 + map[ch], 0)) & 0xdf;
		SetReg(0x20 + i * 3 + map[ch], tmp | (keyon ? 0 : 0x20), 1);
		bool isCarrier = (i == 1) || (voice->AL & 1);
		bool isSustained = (parent && parent->GetSustain());
		uint8_t rr = (isSustained && isCarrier) ? GET_RV(voice, i) : GET_RR(voice, i);
		uint8_t sr = keyon ? GET_SR(voice, i) : rr;
		tmp = (GET_SL(voice, i) << 4) | (sr & 0xf);
		SetReg(0x80 + i * 3 + map[ch], tmp, 1);
	}
	tmp = GetReg(0xb0 + ch, 0) & 0xdf;
	SetReg(0xb0 + ch, tmp | (keyon ? 0x20 : 0), 1);
}

#if 0

void COPL::RhythmOn(uint8_t num, uint8_t vel, int8_t pan, FMVOICE* rv, FNUM* fnum)
{
	//SetReg(0xbd, 0x20);
	if (num < rhythmcap) {
		RhythmOff(num);
		uint8_t evol = CalcLinearLevel(vel, 127-rhythmvol);
		evol = Linear2dB(evol, RANGE48DB, STEP075DB, 6);
		uint16_t addr = RhythmReg[num];
		uint8_t vch = RhythmMapCh[num];

		if (rv && vch < 9) {
			SetVoice(vch, rv);
		}
		fnum = fnum ? fnum : &RhythmFnum[vch-6];
		UpdateFreq(vch, fnum);
		SetReg(addr, evol, 1);
		SetReg(0xbd, (1 << num) | (GetReg(0xbd, 0) & 0xc0) | 0x20);
		//RhythmOnMap |= (1 << num);
	}
}

void COPL::RhythmOff(uint8_t num)
{
	if (num < rhythmcap) {
		SetReg(0xbd, GetReg(0xbd, 0) & ~(1 << num));
		//RhythmOffMap |= (1 << num);
	}
}

#endif

void COPL::TimerCallBack(uint32_t tick)
{
	CSoundDevice::TimerCallBack(tick);
#if 0
	if (RhythmOffMap) {
		SetReg(0xbd, 0x20 | ((~RhythmOffMap) & (GetReg(0x0e, 0) & 0xdf)), 1);
		RhythmOffMap = 0;
	}
	if (RhythmOnMap) {
		SetReg(0xbd, 0x20 | RhythmOnMap, 1);
		RhythmOnMap = 0;
	}
#endif
}

//--------
C3801::C3801(CPort* pt, int fsamp) : COPL(pt, fsamp, DEVICE_Y8950)
{
}

//--------
COPL2::COPL2(CPort* pt, int fsamp) : COPL(pt, fsamp, DEVICE_OPL2)
{
	SetReg(0x01, 0x20, 1);	//Enable WS
}

