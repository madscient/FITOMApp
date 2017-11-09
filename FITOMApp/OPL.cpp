#include "STDAFX.h"
#include "OPL.h"

UINT8 COPL::map[] = { 0, 1, 2, 8, 9, 10, 16, 17, 18, };
UINT8 COPL::RhythmReg[] = { 0x51, 0x55, 0x52, 0x54, 0x53, };
UINT8 COPL::RhythmMapCh[] = { 7, 8, 8, 7, 6, };
ISoundDevice::FNUM COPL::RhythmFnum[] = {	
	ISoundDevice::FNUM(2, 0x480), ISoundDevice::FNUM(2, 0x540), ISoundDevice::FNUM(0, 0x700)
};
UINT8 COPL::RhythmFreq[] = { 47, 60, 53, };

#define GET_AR(v,o)	(v->op[o].AR >> 3)
#define GET_DR(v,o)	(v->op[o].DR >> 3)
#define GET_SR(v,o)	(v->op[o].SR >> 3)
#define GET_RR(v,o)	(v->op[o].RR >> 3)
#define GET_SL(v,o)	(v->op[o].SL >> 3)
#define GET_TL(v,o)	(v->op[o].TL)
//#define GET_RV(v,o)	(v->op[o].REV >> 3)
#define GET_RV(v,o)	(4)


COPL::COPL(CPort* pt, int fsamp, UINT8 devtype)
	: CSoundDevice(devtype, 9, fsamp, 72, FNUM_OFFSET, FnumTableType::Fnumber, pt, 0x100)
	,RhythmOnMap(0), RhythmOffMap(0)
{
	ops = 2;
}

void COPL::Init()
{
	SetReg(0x04, 0, 1);
	SetReg(0x08, 0, 1);
}

void COPL::UpdateVoice(UINT8 ch)
{
	UINT8 tmp;
	UINT8 i = 0;
	UINT8 ex = 0;
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	CMidiCh* parent = attr->GetParent();

	for(i=0; i<2; i++)
	{
		tmp = ((voice->op[i].AM & 0x1) << 7) | ((voice->op[i].VIB & 0x1) << 6) |
			(GET_SR(voice, i)?0:0x20) | ((voice->op[i].KSR & 0x1) << 4) |
			(voice->op[i].MUL & 0x0F);
		SetReg(0x20 + i * 3 + map[ch], tmp);

		tmp = (UINT8)((voice->op[i].KSL << 6) | GET_TL(voice, i));
		SetReg(0x40 + i * 3 + map[ch], tmp);

		tmp = (GET_AR(voice, i) << 4) | GET_DR(voice, i);
		SetReg(0x60 + i * 3 + map[ch], tmp);

		UINT8 rr = ((parent && parent->GetSustain() && ((i==1) || (voice->AL&1))) ?
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

void COPL::UpdateVolExp(UINT8 ch)
{
	UINT8 tl;
	UINT8 tmp;
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	CMidiCh* parent = attr->GetParent();

	UINT8 evol = attr->GetEffectiveLevel();
	if (voice->AL & 1) {
		tl = CalcLinearLevel(evol, voice->op[0].TL);
		attr->baseTL[0] = tl;
		tl = Linear2dB(tl, RANGE48DB, STEP075DB, 6);
		tmp = (UINT8)((voice->op[0].KSL << 6) | tl);
		SetReg(0x40 + map[ch], tmp, 0);
	}
	tl = CalcLinearLevel(evol, voice->op[1].TL);
	attr->baseTL[1] = tl;
	tl = Linear2dB(tl, RANGE48DB, STEP075DB, 6);
	tmp = (UINT8)((voice->op[1].KSL << 6) | tl);
	SetReg(0x43 + map[ch], tmp, 0);
}

void COPL::UpdateFreq(UINT8 ch, const FNUM* fnum)
{
	fnum = fnum ? fnum : GetChAttribute(ch)->GetLastFnumber();
	SetReg(0xa0 + ch, (UINT8)((fnum->fnum>>1) & 0xff), 1);
	SetReg(0xb0 + ch, (GetReg(0xb0 + ch, 0) & 0x20) |
		(UINT8)((fnum->block << 2) | (fnum->fnum >> 9)), 1);
}

void COPL::UpdatePanpot(UINT8 ch)
{
	int pan = (GetChAttribute(ch)->panpot) / 8;
	UINT8 chena = 0;
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

void COPL::UpdateSustain(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	CMidiCh* parent = attr->GetParent();
	if (voice->AL & 1) {
		UINT8 rr = (parent && parent->GetSustain()) ? GET_RV(voice, 0) : (GET_SR(voice, 0) ? GET_SR(voice, 0) : GET_RR(voice, 0));
		UINT8 tmp = (GET_SL(voice, 0) << 4) | (rr & 0xf);
		SetReg(0x80 + map[ch], tmp);
	}
	UINT8 rr = (parent && parent->GetSustain()) ? GET_RV(voice, 1) : (GET_SR(voice, 1) ? GET_SR(voice, 1) : GET_RR(voice, 1));
	UINT8 tmp = (GET_SL(voice, 1) << 4) | (rr & 0xf);
	SetReg(0x83 + map[ch], tmp);
}

void COPL::UpdateTL(UINT8 ch, UINT8 op, UINT8 lev)
{
	lev = (lev > 63) ? 63 : (lev & 63);
	SetReg(0x40 + (3 * op) + map[ch], lev, 0);
}


void COPL::UpdateKey(UINT8 ch, UINT8 keyon)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	CMidiCh* parent = attr->GetParent();
	UINT8 tmp;
	for (int i=0; i<2; i++) {
		tmp = (GetReg(0x20 + i * 3 + map[ch], 0)) & 0xdf;
		SetReg(0x20 + i * 3 + map[ch], tmp | (keyon ? 0 : 0x20), 1);
		BOOL isCarrier = (i == 1) || (voice->AL & 1);
		BOOL isSustained = (parent && parent->GetSustain());
		UINT8 rr = (isSustained && isCarrier) ? GET_RV(voice, i) : GET_RR(voice, i);
		UINT8 sr = keyon ? GET_SR(voice, i) : rr;
		tmp = (GET_SL(voice, i) << 4) | (sr & 0xf);
		SetReg(0x80 + i * 3 + map[ch], tmp, 1);
	}
	tmp = GetReg(0xb0 + ch, 0) & 0xdf;
	SetReg(0xb0 + ch, tmp | (keyon ? 0x20 : 0), 1);
}

#if 0

void COPL::RhythmOn(UINT8 num, UINT8 vel, SINT8 pan, FMVOICE* rv, FNUM* fnum)
{
	//SetReg(0xbd, 0x20);
	if (num < rhythmcap) {
		RhythmOff(num);
		UINT8 evol = CalcLinearLevel(vel, 127-rhythmvol);
		evol = Linear2dB(evol, RANGE48DB, STEP075DB, 6);
		UINT16 addr = RhythmReg[num];
		UINT8 vch = RhythmMapCh[num];

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

void COPL::RhythmOff(UINT8 num)
{
	if (num < rhythmcap) {
		SetReg(0xbd, GetReg(0xbd, 0) & ~(1 << num));
		//RhythmOffMap |= (1 << num);
	}
}

#endif

void COPL::TimerCallBack(UINT32 tick)
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

