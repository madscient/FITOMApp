#include "STDAFX.H"
#include "FITOM.h"
#include "OPLL.h"
#include "MIDI.h"

ISoundDevice::FNUM COPLL::RhythmFnum[3] = {	
	ISoundDevice::FNUM(2, 0x480), ISoundDevice::FNUM(2, 0x540), ISoundDevice::FNUM(0, 0x700)
};
UINT8 COPLL::RhythmReg[] = { 0x37, 0x38, 0x38, 0x37, 0x36, };
UINT8 COPLL::RhythmMapCh[] = { 7, 8, 8, 7, 6, };

UINT8 COPLL::RhythmFreq[3] = { 47, 60, 53, };

#define GET_AR(v,o)	(v->op[o].AR >> 3)
#define GET_DR(v,o)	(v->op[o].DR >> 3)
#define GET_SR(v,o)	(v->op[o].SR >> 3)
#define GET_RR(v,o)	(v->op[o].RR >> 3)
#define GET_SL(v,o)	(v->op[o].SL >> 3)
#define GET_TL(v,o)	(v->op[o].TL)
//#define GET_RV(v,o)	(v->op[o].REV >> 3)
#define GET_RV(v,o)	(4)
#define IS_KEY(ch) (GetReg(0x20 + ch, 0) & 0x10)

COPLL::COPLL(CPort* pt, UINT8 mode, int fsamp, UINT8 devtype) :
CSoundDevice(devtype, 9, fsamp, 72, FNUM_OFFSET, FnumTableType::Fnumber, pt, 0x40), RhythmOnMap(0), RhythmOffMap(0)
{
	ops = 2;
	if (mode) {
		rhythmcap = 5;
		for (int i=6; i<9; i++) {
			EnableCh(i, 0);
		}
	}
}

void COPLL::SetVoice(UINT8 ch, FMVOICE* voice, int update)
{
	update = (voice->AL & 0x40) ? update : -1;
	CSoundDevice::SetVoice(ch, voice, update);
}

void COPLL::UpdateVoice(UINT8 ch)
{
	UINT8 tmp;
	FMVOICE* voice = GetChAttribute(ch)->GetVoice();

	UINT8 inst = 0;
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

void COPLL::UpdateVolExp(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	UINT8 evol = CalcLinearLevel(attr->GetEffectiveLevel(), voice->op[1].TL);
	attr->baseTL[1] = evol;
	evol = Linear2dB(evol, RANGE48DB, STEP150DB, 4);
	SetReg(0x30 + ch, (GetReg(0x30 + ch, 0) & 0xf0) | evol, 0);
}

void COPLL::UpdateFreq(UINT8 ch, const FNUM* fnum)
{
	CHATTR* attr = GetChAttribute(ch);
	fnum = fnum ? fnum : attr->GetLastFnumber();
	SetReg(0x10 + ch, UINT8((fnum->fnum>>2) & 0xff), 1);
	SetReg(0x20 + ch, (GetReg(0x20 + ch, 0) & 0x30) |
		(fnum->block << 1) | UINT8(fnum->fnum >> 10), 1);
}

void COPLL::UpdateSustain(UINT8 ch)
{
	UINT8 sus = GetChAttribute(ch)->GetParent()->GetSustain() ? 0x20 : 0;
	UINT8 tmp = GetReg(0x20 + ch, 0) & 0xdf;
	SetReg(0x20 + ch, sus | tmp);
}

void COPLL::UpdateTL(UINT8 ch, UINT8 op, UINT8 lev)
{
	lev = (lev > 63) ? 63 : lev & 0x3f;
	if (op == 0) {
		SetReg(0x2, (GetReg(2, 0) & 0xc0) | lev, 0);
	} else if (op == 1) {
		SetReg(0x30 + ch, (GetReg(0x30 + ch, 0) & 0xf0) | (lev >> 2), 0);
	}
}

void COPLL::UpdateKey(UINT8 ch, UINT8 keyon)
{
	UINT8 tmp;
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

void COPLL::RhythmOn(UINT8 num, UINT8 vel, SINT8 pan, FMVOICE* rv, FNUM* fnum)
 {
	//SetReg(0x0e, 0x20);
	if (num < rhythmcap) {
		RhythmOff(num);
		UINT8 evol = Linear2dB(CalcLinearLevel(rhythmvol, 127 - vel), RANGE48DB, STEP150DB, 4);
		UINT16 addr = RhythmReg[num];
		UINT8 vch = RhythmMapCh[num];
		UINT8 mask = (num&5) ? 0xf0 : 0x0f;
		evol = (num&5) ? evol : (evol << 4);

		SetReg(addr, (GetReg(addr, 0) & mask) | evol);
		fnum = fnum ? fnum : &RhythmFnum[vch-6];
		UpdateFreq(vch, fnum);
		SetReg(0x0e, (1 << num) | 0x20);
		//RhythmOnMap |= (1 << num);
	}
}

void COPLL::RhythmOff(UINT8 num)
{
	if (num < rhythmcap) {
		SetReg(0x0e, GetReg(0x0e, 0) & ~(1 << num));
		//RhythmOffMap |= (1 << num);
	}
}

void COPLL::TimerCallBack(UINT32 tick)
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

COPLLP::COPLLP(CPort* pt, UINT8 mode, int fsamp) : COPLL(pt, mode, fsamp, DEVICE_OPLLP)
{
}

COPLLX::COPLLX(CPort* pt, UINT8 mode, int fsamp) : COPLL(pt, mode, fsamp, DEVICE_OPLLX)
{
}

COPLL2::COPLL2(CPort* pt, UINT8 mode, int fsamp) : COPLL(pt, mode, fsamp, DEVICE_OPLL2)
{
}

void COPLL2::UpdateFreq(UINT8 ch, const FNUM* fnum)
{
	CHATTR* attr = GetChAttribute(ch);
	fnum = fnum ? fnum : attr->GetLastFnumber();
	SetReg(0x10 + ch, (fnum->block << 5)|UINT8((fnum->fnum>>6)&0xff), 0);
	SetReg(0x20 + ch, (GetReg(0x20 + ch, 0) & 0xf0) | UINT8((fnum->fnum>>2)&0xf), 0);
}
