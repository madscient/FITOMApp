#include "STDAFX.H"
#include "OPLL.h"

#define GET_AR(v,o)	(v->op[o].AR >> 3)
#define GET_DR(v,o)	(v->op[o].DR >> 3)
#define GET_SR(v,o)	(v->op[o].SR >> 3)
#define GET_RR(v,o)	(v->op[o].RR >> 3)
#define GET_SL(v,o)	(v->op[o].SL >> 3)
#define GET_TL(v,o)	(v->op[o].TL)
//#define GET_RV(v,o)	(v->op[o].REV >> 3)
#define GET_RV(v,o)	(4)
#define IS_KEY(ch) (GetReg(0x20 + ch, 0) & 0x10)

COPLL::COPLL(CPort* pt, uint8_t mode, int fsamp, uint8_t devtype) :
CSoundDevice(devtype, 9, fsamp, 72, FNUM_OFFSET, FnumTableType::Fnumber, pt, 0x40)
{
	ops = 2;
	ChangeRhythmMode(mode);
}

void COPLL::ChangeRhythmMode(int mode)
{
	for (int i = 6; i<9; i++) {
		EnableCh(i, (mode == 0));
	}
}

void COPLL::Init()
{
}

void COPLL::SetVoice(uint8_t ch, FMVOICE* voice, int update)
{
	update = (voice->AL & 0x40) ? update : -1;
	CSoundDevice::SetVoice(ch, voice, update);
}

void COPLL::UpdateVoice(uint8_t ch)
{
	uint8_t tmp;
	FMVOICE* voice = GetChAttribute(ch)->GetVoice();

	uint8_t inst = 0;
	if (voice->AL & 0x40) {//Preset Instrument
		inst = voice->AL & 0xf;
	} else {//User Instrument
		for (int i=0; i<2; i++) {
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

void COPLL::UpdateVolExp(uint8_t ch)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	uint8_t evol = CalcLinearLevel(attr->GetEffectiveLevel(), voice->op[1].TL);
	attr->baseTL[1] = evol;
	evol = Linear2dB(evol, RANGE48DB, STEP150DB, 4);
	SetReg(0x30 + ch, (GetReg(0x30 + ch, 0) & 0xf0) | evol, 0);
}

void COPLL::UpdateFreq(uint8_t ch, const FNUM* fnum)
{
	CHATTR* attr = GetChAttribute(ch);
	fnum = fnum ? fnum : attr->GetLastFnumber();
	SetReg(0x10 + ch, uint8_t((fnum->fnum>>2) & 0xff), 1);
	SetReg(0x20 + ch, (GetReg(0x20 + ch, 0) & 0x30) |
		(fnum->block << 1) | uint8_t(fnum->fnum >> 10), 1);
}

void COPLL::UpdateSustain(uint8_t ch)
{
	uint8_t sus = GetChAttribute(ch)->GetParent()->GetSustain() ? 0x20 : 0;
	uint8_t tmp = GetReg(0x20 + ch, 0) & 0xdf;
	SetReg(0x20 + ch, sus | tmp);
}

void COPLL::UpdateTL(uint8_t ch, uint8_t op, uint8_t lev)
{
	lev = (lev > 63) ? 63 : lev & 0x3f;
	if (op == 0) {
		SetReg(0x2, (GetReg(2, 0) & 0xc0) | lev, 0);
	} else if (op == 1) {
		SetReg(0x30 + ch, (GetReg(0x30 + ch, 0) & 0xf0) | (lev >> 2), 0);
	}
}

void COPLL::UpdateKey(uint8_t ch, uint8_t keyon)
{
	uint8_t tmp;
	FMVOICE* voice = GetChAttribute(ch)->GetVoice();

	if (!(voice->AL & 0x40)) {//Preset Instrument
		for (int i=0; i<2; i++) {
			tmp = (GET_SL(voice, i) << 4) | (((keyon && GET_SR(voice, i)) ? GET_SR(voice, i) : GET_RR(voice, i)) & 0xf);
			SetReg(0 + i, (GetReg(1, 0) & 0xdf) | ((keyon && GET_SR(voice, i)) ? 0 : 0x20));
			SetReg(6 + i, tmp);
		}
	}

	tmp = GetReg(0x20 + ch, 0) & 0xef;
	SetReg(0x20 + ch, tmp | (keyon ? 0x10 : 0));
}

void COPLL::TimerCallBack(uint32_t tick)
{
	CSoundDevice::TimerCallBack(tick);
#if 0
	if (RhythmOffMap) {
		SetReg(0x0e, 0x20 | ((~RhythmOffMap) & (GetReg(0x0e, 0) & 0x1f)));
		RhythmOffMap = 0;
	}
	if (RhythmOnMap) {
		SetReg(0x0e, 0x20 | RhythmOnMap);
		RhythmOnMap = 0;
	}
#endif
}

COPLLP::COPLLP(CPort* pt, uint8_t mode, int fsamp) : COPLL(pt, mode, fsamp, DEVICE_OPLLP)
{
}

COPLLX::COPLLX(CPort* pt, uint8_t mode, int fsamp) : COPLL(pt, mode, fsamp, DEVICE_OPLLX)
{
}

COPLL2::COPLL2(CPort* pt, uint8_t mode, int fsamp) : COPLL(pt, mode, fsamp, DEVICE_OPLL2)
{
}

void COPLL2::UpdateFreq(uint8_t ch, const FNUM* fnum)
{
	CHATTR* attr = GetChAttribute(ch);
	fnum = fnum ? fnum : attr->GetLastFnumber();
	SetReg(0x10 + ch, (fnum->block << 5)|uint8_t((fnum->fnum>>6)&0xff), 0);
	SetReg(0x20 + ch, (GetReg(0x20 + ch, 0) & 0xf0) | uint8_t((fnum->fnum>>2)&0xf), 0);
}


//-----
ISoundDevice::FNUM COPLLRhythm::RhythmFnum[3] = {
	ISoundDevice::FNUM(2, 0x480), ISoundDevice::FNUM(2, 0x540), ISoundDevice::FNUM(0, 0x700)
};
uint8_t COPLLRhythm::RhythmReg[] = { 0x37, 0x38, 0x38, 0x37, 0x36, };
uint8_t COPLLRhythm::RhythmMapCh[] = { 7, 8, 8, 7, 6, };

uint8_t COPLLRhythm::RhythmFreq[3] = { 47, 60, 53, };


COPLLRhythm::COPLLRhythm(COPLL* parent) : CRhythmDevice(parent, DEVICE_OPLL_RHY, 5)
{
	parent->ChangeRhythmMode(1);
}

void COPLLRhythm::Init()
{
	SetReg(0x0e, 0x20);
}

void COPLLRhythm::UpdateVolExp(uint8_t ch)
{
	CHATTR* attr = GetChAttribute(ch);
	//SetReg(0x0e, 0x20);
	if (attr) {
		uint8_t evol = Linear2dB(CalcLinearLevel(attr->GetVolume(), 127 - attr->velocity), RANGE48DB, STEP150DB, 4);
		uint16_t addr = RhythmReg[ch];
		uint8_t vch = RhythmMapCh[ch];
		uint8_t mask = (ch & 5) ? 0xf0 : 0x0f;
		evol = (ch & 5) ? evol : (evol << 4);

		SetReg(addr, (GetReg(addr, 0) & mask) | evol);
		//RhythmOnMap |= (1 << num);
	}
}

void COPLLRhythm::UpdateFreq(uint8_t ch, const FNUM* fnum)
{
	uint8_t vch = RhythmMapCh[ch];
	fnum = fnum ? fnum : &RhythmFnum[vch - 6];
	((COPLL*)pParent)->UpdateFreq(vch, fnum);
}

void COPLLRhythm::UpdateKey(uint8_t ch, uint8_t keyon)
{
	if (ch < chs) {
		uint8_t keymask = ~(1 << ch);
		SetReg(0x0e, (GetReg(0x0e, 0) & keymask) | 0x20);
		if (keyon) {
			SetReg(0x0e, (GetReg(0x0e, 0) & keymask) | 0x20 | (1 << ch));
		}
	}
}

uint8_t COPLLRhythm::QueryCh(CMidiCh* parent, FMVOICE* voice, int mode)
{
	uint8_t ret = 0xff;
	if (voice) {
		int num = voice->AL & 0x7;
		int inuse = GetReg(0x0e, 0) & (1 << num);
		ret = mode ? num : (inuse ? 0xff : num);
	}
	return ret;
}
