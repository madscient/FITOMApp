#include "stdafx.h"
#include "SSG.h"
#include "FITOM.h"
#include "MIDI.h"

CPSGBase::CPSGBase(UINT8 devid, CPort* pt, UINT8 ch, int fsamp)
	: CSoundDevice(devid, ch, fsamp, 2, -576, FnumTableType::TonePeriod, pt)
{
	ops = 0;
	lfoTL = new UINT8[chs];
	egattr = new CEnvelope[chs];
}

ISoundDevice::FNUM CPSGBase::GetFnumber(UINT8 ch, SINT16 offset)
{
	FNUM ret;
	CHATTR* attr = GetChAttribute(ch);
	int index = attr->GetNoteIndex(MasterTune + (NoteOffset * 64) + offset);
	int oct = (-NoteOffset / 12) - 1;

	// normalize
	while (index < 0) {
		oct--;
		index += 768;
	}
	while (index >= 768) {
		oct++;
		index -= 768;
	}

	ret.block = UINT8(oct);
	ret.fnum = Fnum[index];
	return ret;
}

void CPSGBase::UpdateKey(UINT8 ch, UINT8 keyon)
{
	FMVOICE* voice = GetChAttribute(ch)->GetVoice();
	if (keyon) {
		UpdateVoice(ch);
		if (!(voice->AL & 0x4)) {
			egattr[ch].Start(&voice->op[0]);
		}
	} else {
		egattr[ch].Release();
	}
}

void CPSGBase::PollingCallBack()
{
}

void CPSGBase::UpdateLevelEG()
{
	for (int i=0; i<chs; i++) {
		if (egattr[i].GetPhase() != CEnvelope::EG_NONE) {
			egattr[i].Update();
		} else if (GetChAttribute(i)->IsRunning()) {
			NoteOff(i);
		}
	}
}

void CPSGBase::TimerCallBack(UINT32 tick)
{
	if (/*(tick & 1)*/1) {
		UpdateLevelEG();
	}
	CSoundDevice::TimerCallBack(tick);

	for (int i=0; i<chs; i++) {
		if (egattr[i].GetPhase() != CEnvelope::EG_NONE) {
			UpdateVolExp(i);
		}
	}
}

//-------------------------------
CSSG::CSSG(CPort* pt, int fsamp) : CPSGBase(DEVICE_SSG, pt, 3, fsamp)
{
	SetReg(0x07, 0x3f, 1);
	ops = 2;
}

void CSSG::UpdateVolExp(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	if (!(attr->GetVoice()->op[0].EGT & 0x8)) {
		UINT8 evol = attr->GetEffectiveLevel();
		SINT16 lev = SINT16(lfoTL[ch]) - 64 + egattr[ch].GetValue();
		lev = (lev < 0) ? 0 : lev;
		lev = (lev > 127) ? 127 : lev;
		evol = 15 - Linear2dB(CalcLinearLevel(evol, 127-UINT8(lev)), RANGE48DB, STEP300DB, 4);
		SetReg(8 + ch, evol & 0xf, 1);
	}
}

void CSSG::UpdateFreq(UINT8 ch, const FNUM* fnum)
{
	fnum = fnum ? fnum : GetChAttribute(ch)->GetLastFnumber();
	UINT8 oct = fnum->block;
	UINT16 etp = fnum->fnum >> (oct + 2);
	SetReg(ch * 2 + 0, UINT8(etp & 0xff), 1);
	SetReg(ch * 2 + 1, UINT8(etp >> 8), 1);
}

void CSSG::UpdateVoice(UINT8 ch)
{
	UINT8 mix = 8;
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	switch (voice->AL & 3) {
	case 0:
		mix = 0x8;
		break;
	case 1:
		mix = 0x1;
		break;
	case 2:
		mix = 0x0;
		break;
	case 3:
		mix = 0x9;
		break;
	}
	lfoTL[ch] = attr->baseTL[0] = attr->baseTL[1] = 64;
	SetReg(0x7, (GetReg(0x7, 0) & ~(0x9 << ch)) | (mix << ch), 1);
	if (mix==1 || mix==0) {
		SetReg(0x6, voice->NFQ, 1);
	}
	if (voice->op[0].EGT & 0x8) {	//HW env
		SetReg(0x8+ch, (GetReg(0x8+ch, 0) & 0xe0) | 0x10, 1);
		SetReg(0xb, (((voice->op[0].SR << 4) & 0xf0) | (voice->op[0].SL & 0xf)), 1);
		SetReg(0xc, (((voice->op[0].DR << 2) & 0xfc) | ((voice->op[0].SR >> 4) & 0x3)), 1);
		SetReg(0xd,  (voice->op[0].EGT & 0xf), 1);
	}
}

void CSSG::UpdateTL(UINT8 ch, UINT8 op, UINT8 lev)
{
	FMVOICE* voice = GetChAttribute(ch)->GetVoice();
	switch (op) {
	case 0:// Amplitude LFO
		{
			lfoTL[ch] = lev;
		}
		break;
	case 1:// Noise freq LFO
		if ((voice->AL & 3)==1 || (voice->AL & 3)==2) {
			SINT16 frq = SINT16(lev) - 64 + (((voice->NFQ << 2) | (voice->NFQ >> 3)) & 0x7f);
			frq = (frq < 0) ? 0 : frq;
			frq = (frq > 127) ? 127 : frq;
			SetReg(0x6, lev >> 2, 1);
		}
		break;
	}
}

UINT8 CSSG::QueryCh(CMidiCh* parent, FMVOICE* voice, int mode)
{
	CHATTR* attr2 = GetChAttribute(2);
	CHATTR* attr1 = GetChAttribute(1);
	if (voice && (voice->AL & 0x3) != 0) {//Noise enabled
		if (mode ? attr2->IsAvailable() : attr2->IsEnable()) {
			return 2;
		} else {
			return 0xff;
		}
	} else if (voice && (voice->AL & 0x20) != 0) {//HW Envelop
		if (mode ? attr1->IsAvailable() : attr1->IsEnable()) {
			return 1;
		} else {
			return 0xff;
		}
	}
	return CSoundDevice::QueryCh(parent, voice, mode);
}
//-------------------------------
CPSG::CPSG(CPort* pt, int fsamp) : CSSG(pt, fsamp)
{
	SetDevice(DEVICE_PSG);
}

//-------------------------------
CSSGS::CSSGS(CPort* pt, int fsamp) : CPSGBase(DEVICE_SSGS, pt, 6, fsamp)
{
	SetReg(0x07, 0x3f, 1);
	ops = 2;
}

void CSSGS::UpdateVolExp(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	if (!(voice->op[0].EGT & 0x8)) {
		UINT8 evol = attr->GetEffectiveLevel();
		SINT16 lev = SINT16(lfoTL[ch]) - 64 + egattr[ch].GetValue();
		lev = (lev < 0) ? 0 : lev;
		lev = (lev > 127) ? 127 : lev;
		evol = Linear2dB(CalcLinearLevel(evol, 127-UINT8(lev)), RANGE48DB, STEP300DB, 4);
		UINT8 off = (ch < 3) ? 8 : 0x28;
		UINT8 dch = (ch < 3) ? ch : (ch-3);
		SetReg(off + dch, 15-evol & 0xf, 1);
	}
}

void CSSGS::UpdateFreq(UINT8 ch, const FNUM* fnum)
{
	fnum = fnum ? fnum : GetChAttribute(ch)->GetLastFnumber();
	UINT8 oct = fnum->block;
	UINT16 etp = fnum->fnum >> (oct + 2);
	UINT8 off = (ch < 3) ? 0 : 0x20;
	UINT8 dch = (ch < 3) ? ch : (ch-3);
	SetReg(off + dch * 2 + 0, UINT8(etp & 0xff), 1);
	SetReg(off + dch * 2 + 1, UINT8(etp >> 8), 1);
}

void CSSGS::UpdateVoice(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	UINT8 mix = 8;
	switch (voice->AL & 3) {
	case 0:
		mix = 0x8;	//tone
		break;
	case 1:
		mix = 0x1;	//noise
		break;
	case 2:
		mix = 0x0;	//tone+noise
		break;
	case 3:
		mix = 0x9;	//off
		break;
	}
	lfoTL[ch] = attr->baseTL[0] = attr->baseTL[1] = 64;
	UINT8 off = (ch < 3) ? 0 : 0x20;
	UINT8 dch = (ch < 3) ? ch : (ch-3);
	SetReg(off + 0x7, (GetReg(0x7, 0) & ~(0x9 << dch)) | (mix << dch), 1);
	if (mix==0 || mix==1) {
		SetReg(off + 0x6, voice->NFQ, 1);
	}
	if (voice->op[0].EGT & 0x8) {	//hw env
		SetReg(off + 0x8 + dch, (GetReg(0x8+ch, 0) & 0xe0) | 0x10, 1);
		SetReg(off + 0xb, (((voice->op[0].SR << 4) & 0xf0) | (voice->op[0].SL & 0xf)), 1);
		SetReg(off + 0xc, (((voice->op[0].DR << 2) & 0xfc) | ((voice->op[0].SR >> 4) & 0x3)), 1);
		SetReg(0xd,  (voice->op[0].EGT & 0xf), 1);
	}
}

void CSSGS::UpdateTL(UINT8 ch, UINT8 op, UINT8 lev)
{
	FMVOICE* voice = GetChAttribute(ch)->GetVoice();
	switch (op) {
	case 0:// Amplitude LFO
		{
			lfoTL[ch] = lev;
		}
		break;
	case 1:// Noise freq LFO
		if ((voice->AL & 3)==1 || (voice->AL & 3)==2) {
			SINT16 frq = SINT16(lev) - 64 + (((voice->NFQ << 2) | (voice->NFQ >> 3)) & 0x7f);
			frq = (frq < 0) ? 0 : frq;
			frq = (frq > 127) ? 127 : frq;
			UINT8 off = (ch < 3) ? 0 : 0x20;
			SetReg(off + 0x6, lev >> 2, 1);
		}
		break;
	}
}

void CSSGS::UpdatePanpot(UINT8 ch)
{
	UINT8 off = (ch < 3) ? 0 : 0x20;
	UINT8 dch = (ch < 3) ? ch : (ch-3);
	SetReg(off + 0x10 + dch, (GetChAttribute(ch)->panpot >> 2), 1);
}

UINT8 CSSGS::QueryCh(CMidiCh* parent, FMVOICE* voice, int mode)
{
	CHATTR* attr = 0;
	if (voice && ((voice->AL & 3)==1 || (voice->AL & 3)==2)) {//Noise enabled
		attr = GetChAttribute(2);
		if (mode ? attr->IsAvailable() : attr->IsEnable()) {
			return 2;
		}
		attr = GetChAttribute(5);
		if (mode ? attr->IsAvailable() : attr->IsEnable()) {
			return 5;
		}
		return 0xff;
	} else if (voice && (voice->AL & 0x4) != 0) {//HW Envelop
		attr = GetChAttribute(1);
		if (mode ? attr->IsAvailable() : attr->IsEnable()) {
			return 1;
		}
		attr = GetChAttribute(4);
		if (mode ? attr->IsAvailable() : attr->IsEnable()) {
			return 4;
		}
		return 0xff;
	}
	return CSoundDevice::QueryCh(parent, voice, mode);
}

//-------------------------------
CDCSG::CDCSG(CPort* pt, int fsamp) : CPSGBase(DEVICE_DCSG, pt, 4, fsamp)
{
	port->write(0, 0x80, 1);
	port->write(0, 0x00, 1);
	port->write(0, 0x9f, 1);
	port->write(0, 0xa0, 1);
	port->write(0, 0x00, 1);
	port->write(0, 0xbf, 1);
	port->write(0, 0xc0, 1);
	port->write(0, 0x00, 1);
	port->write(0, 0xdf, 1);
	port->write(0, 0xe0, 1);
	port->write(0, 0xff, 1);
}

void CDCSG::UpdateVolExp(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	UINT8 evol = attr->GetEffectiveLevel();
	evol = Linear2dB(CalcLinearLevel(evol, 127 - egattr[ch].GetValue()), RANGE48DB, STEP300DB, 4);
	if (ch < 3) {
		port->writeRaw(0, 0x90 | (ch * 32) | (evol & 0xf));
	}
	else {
		port->writeRaw(0, 0xf0 | (evol & 0xf));
	}
}

void CDCSG::UpdateFreq(UINT8 ch, const FNUM* fnum)
{
	fnum = fnum ? fnum : GetChAttribute(ch)->GetLastFnumber();
	if (ch < 3) {
		UINT8 oct = fnum->block;
		UINT16 etp = fnum->fnum >> (oct + 4);
		port->writeRaw(0, 0x80 | (ch * 32) | (etp & 0xf));
		port->writeRaw(0, 0x0 | ((etp >> 4) & 63));
	}
	else {
	}
}

void CDCSG::UpdateVoice(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	switch (ch) {
	case 0:
	case 1:
		break;
	case 2:
		/*
		if (attr->GetVoice()->AL == 2) {
			port->write(0, 0xe2 | (attr->GetVoice()->NFQ & 0x3), 1);
		}
		*/
		break;
	case 3:
		if (attr->GetVoice()->AL == 1) {
			port->writeRaw(0, 0xe0 | ((attr->GetVoice()->FB & 1) << 2) | (attr->GetVoice()->NFQ & 0x3));
		}
		break;
	}
}

UINT8 CDCSG::QueryCh(CMidiCh* parent, FMVOICE* voice, int mode)
{
	UINT8 ret = 0xff;
	if (voice && voice->AL == 1) {
		CHATTR* attr = GetChAttribute(3);
		if (mode ? attr->IsAvailable() : attr->IsEnable()) {
			ret = 3;
		}
	}
	/*
	if (voice && voice->AL == 2) {
		CHATTR* attr = GetChAttribute(2);
		if (mode ? attr->IsAvailable() : attr->IsEnable()) {
			ret = 2;
		}
	}
	*/
	ret = CSoundDevice::QueryCh(parent, voice, mode);
	if (voice && voice->AL == 0) {
		if (ret == 3) {
			ret = 0;
			for (int i = 0; i < 3; i++) {
				CHATTR* attr = GetChAttribute(i);
				if (attr->IsAvailable()) {
					ret = i;
				}
			}
		}
	}
	return ret;
}


UINT8 CDCSG::AllocCh(CMidiCh* par, FMVOICE* voice)
{
	return CSoundDevice::AllocCh(par, voice);
}


//-------------------------------

CEPSG::CEPSG(CPort* pt, int fsamp) : CPSGBase(DEVICE_EPSG, pt, 3, fsamp), prevmix(0x3f)
{
	SetReg(0xd, 0xb0, 1);	//Enable expand mode
	SetReg(0x9, 0xff, 1);	//Noise "AND" mask
	SetReg(0xa, 0x00, 1);	//Noise "OR" mask
	SetReg(0x04, 0, 1);
	SetReg(0x05, 0, 1);
	SetReg(0xd, 0xa0, 1);	//Bank select A
	SetReg(0x07, 0x3f, 1);
	SetReg(0x08, 0, 1);
	SetReg(0x09, 0, 1);
	SetReg(0x0a, 0, 1);
	ops = 2;
}

void CEPSG::UpdateVolExp(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	if (!(attr->GetVoice()->op[0].EGT & 0x8)) {
		UINT8 evol = attr->GetEffectiveLevel();
		SINT16 lev = SINT16(lfoTL[ch]) - 64 + egattr[ch].GetValue();
		lev = (lev < 0) ? 0 : lev;
		lev = (lev > 127) ? 127 : lev;
		evol = 31 - Linear2dB(CalcLinearLevel(evol, 127-UINT8(lev)), RANGE48DB, STEP150DB, 5);
		//SetReg(0xd, 0xa0 | (GetReg(0xd, 0) & 0xf), 1);	//Bank select A
		SetReg(8 + ch, evol & 0x1f, 1);
	}
}

void CEPSG::UpdateFreq(UINT8 ch, const FNUM* fnum)
{
	fnum = fnum ? fnum : GetChAttribute(ch)->GetLastFnumber();
	UINT8 oct = fnum->block;
	UINT16 etp = fnum->fnum >> oct;
	//SetReg(0xd, 0xa0 | (GetReg(0xd, 0) & 0xf), 1);	//Bank select A
	SetReg(ch * 2 + 0, UINT8(etp & 0xff), 1);
	SetReg(ch * 2 + 1, UINT8(etp >> 8), 1);
}

void CEPSG::UpdateVoice(UINT8 ch)
{
	UINT8 mix = 8;
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	switch (voice->AL & 3) {
	case 0:
		mix = 0x8;
		break;
	case 1:
		mix = 0x1;
		break;
	case 2:
		mix = 0x0;
		break;
	case 3:
		mix = 0x9;
		break;
	}
	lfoTL[ch] = attr->baseTL[0] = attr->baseTL[1] = 64;
	//SetReg(0xd, 0xa0 | (GetReg(0xd, 0) & 0xf), 1);	//Bank select A
	prevmix = (prevmix & ~(0x9 << ch)) | (mix << ch);
	SetReg(0x7, prevmix, 1);
	if (mix==1 || mix==0) {
		SetReg(0x6, (voice->NFQ << 3) | (voice->FB & 0x7), 1);
	}

	bool hwenv23 = false;
	if (voice->op[0].EGT & 0x8) {	//HW env
		SetReg(0x8+ch, 0x20, 1);
		if (ch == 0) {
			SetReg(0xd, 0xa0 | (voice->op[0].EGT & 0xf), 1);
			SetReg(0xb, (((voice->op[0].SR << 4) & 0xf0) | (voice->op[0].SL & 0xf)), 1);
			SetReg(0xc, (((voice->op[0].DR << 2) & 0xfc) | ((voice->op[0].SR >> 4) & 0x3)), 1);
		} else {
			hwenv23 = true;
		}
	}
	//Duty ratio
	SetReg(0xd, 0xb0 | (GetReg(0xd, 0) & 0xf), 1);	//Bank select B
	SetReg(0x6 + ch, voice->op[0].WS & 0xf, 1);
	if (hwenv23) {
		SetReg(0x0 + (ch - 1) * 2, (((voice->op[0].SR << 4) & 0xf0) | (voice->op[0].SL & 0xf)), 1);
		SetReg(0x1 + (ch - 1) * 2, (((voice->op[0].DR << 2) & 0xfc) | ((voice->op[0].SR >> 4) & 0x3)), 1);
		SetReg(0x3 + ch, (voice->op[0].EGT & 0xf), 1);
	}
	SetReg(0xd, 0xa0 | (GetReg(0xd, 0) & 0xf), 1);	//Bank select A
}

void CEPSG::UpdateTL(UINT8 ch, UINT8 op, UINT8 lev)
{
	FMVOICE* voice = GetChAttribute(ch)->GetVoice();
	switch (op) {
	case 0:// Amplitude LFO
		{
			lfoTL[ch] = lev;
		}
		break;
	case 1:// Noise freq LFO
		if ((voice->AL & 3)==1 || (voice->AL & 3)==2) {
			SINT16 frq = SINT16(lev) - 64 + (((voice->NFQ << 2) | (voice->FB >> 1)) & 0x7f);
			frq = (frq < 0) ? 0 : frq;
			frq = (frq > 127) ? 127 : frq;
			//SetReg(0xd, 0xa0 | (GetReg(0xd, 0) | 0xf), 1);	//Bank select A
			SetReg(0x6, lev, 1);
		}
		break;
	}
}

//-------
CDSG::CDSG(CPort* pt, int fsamp) : CSoundDevice(DEVICE_DSG, 4, fsamp, 2, -576, FnumTableType::TonePeriod, pt)
{
	SetReg(0x07, 0x3f, 1);
	ops = 1;
}

void CDSG::UpdateVolExp(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	UINT8 evol = attr->GetEffectiveLevel();

	SetReg(0xc + ch, (GetReg(0xc + ch, 0) & 0x30) | ((evol & 0x60) >> 1));
}

void CDSG::UpdateFreq(UINT8 ch, const FNUM* fnum)
{
	fnum = fnum ? fnum : GetChAttribute(ch)->GetLastFnumber();
	UINT8 oct = (fnum->block < 4) ? fnum->block : 3;
	UINT16 etp = fnum->fnum >> ((fnum->block < 4) ? 0 : (fnum->block - 3));
	SetReg(0x0 + ch, etp & 0x7f, 1);
	SetReg(0x4 + ch, ((etp >> 7) | oct << 3 | (GetReg(0x4 + ch, 0) & 0x60)), 1);
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
	SetReg(0xc + ch, tmp | ((lev >> 1) & 0x30), 1);
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
	SetReg(0xc + ch, tmp | chena, 1);
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
		SetReg(0x10, (~(1 << num) & (GetReg(0x10, 0) & 0x1f)));
	}
}

void CDSG::SetReg(UINT16 addr, UINT8 data, int v)
{
	if (port) {
		port->write(0, 0x80 | addr, 1);
		port->write(0, data, 1);
	}
}

//-------------------------------
CSCC::CSCC(CPort* pt, int fsamp) : CPSGBase(DEVICE_SCC, pt, 5, fsamp)
{
	ops = 1;
	pt->Mapping(0x9800);
	pt->writeRaw(0xbffe, 0);	// compatible mode (if SCC+)
	pt->writeRaw(0x9000, 0x3f);	// SCC register bank select
}

void CSCC::UpdateVolExp(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	if (!(attr->GetVoice()->op[0].EGT & 0x8)) {
		UINT8 evol = attr->GetEffectiveLevel();
		SINT16 lev = SINT16(lfoTL[ch]) - 64 + egattr[ch].GetValue();
		lev = (lev < 0) ? 0 : lev;
		lev = (lev > 127) ? 127 : lev;
		evol = 15 - Linear2dB(CalcLinearLevel(evol, 127 - UINT8(lev)), RANGE48DB, STEP300DB, 4);
		SetReg(0x8a + ch, evol & 0xf, 1);
	}
}

void CSCC::UpdateFreq(UINT8 ch, const FNUM* fnum)
{
	fnum = fnum ? fnum : GetChAttribute(ch)->GetLastFnumber();
	UINT8 oct = fnum->block;
	UINT16 etp = fnum->fnum >> (oct + 2);
	SetReg(ch * 2 + 0x80, UINT8(etp & 0xff), 1);
	SetReg(ch * 2 + 0x81, UINT8(etp >> 8), 1);
}

void CSCC::UpdateVoice(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	lfoTL[ch] = attr->baseTL[0] = 64;
	SetReg(0x7, (GetReg(0x7, 0) & ~(0x9 << ch)) | (mix << ch), 1);
}

void CSCC::UpdateTL(UINT8 ch, UINT8 op, UINT8 lev)
{
	FMVOICE* voice = GetChAttribute(ch)->GetVoice();
	switch (op) {
	case 0:// Amplitude LFO
	{
		lfoTL[ch] = lev;
	}
	break;
	case 1:// Noise freq LFO
		if ((voice->AL & 3) == 1 || (voice->AL & 3) == 2) {
			SINT16 frq = SINT16(lev) - 64 + (((voice->NFQ << 2) | (voice->NFQ >> 3)) & 0x7f);
			frq = (frq < 0) ? 0 : frq;
			frq = (frq > 127) ? 127 : frq;
			SetReg(0x6, lev >> 2, 1);
		}
		break;
	}
}

UINT8 CSCC::QueryCh(CMidiCh* parent, FMVOICE* voice, int mode)
{
	CHATTR* attr2 = GetChAttribute(2);
	CHATTR* attr1 = GetChAttribute(1);
	if (voice && (voice->AL & 0x3) != 0) {//Noise enabled
		if (mode ? attr2->IsAvailable() : attr2->IsEnable()) {
			return 2;
		}
		else {
			return 0xff;
		}
	}
	else if (voice && (voice->AL & 0x20) != 0) {//HW Envelop
		if (mode ? attr1->IsAvailable() : attr1->IsEnable()) {
			return 1;
		}
		else {
			return 0xff;
		}
	}
	return CSoundDevice::QueryCh(parent, voice, mode);
}
