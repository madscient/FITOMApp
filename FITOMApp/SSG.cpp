#include "stdafx.h"
#include "SSG.h"
#include "FITOM.h"
#include "MIDI.h"

CPSGBase::CPSGBase(UINT8 devid, CPort* pt, int regsize, UINT8 ch, int fsamp)
	: CPSGBase(devid, pt, regsize, ch, fsamp, 1, -576, FnumTableType::TonePeriod)
//	: CSoundDevice(devid, ch, fsamp, 1, -576, FnumTableType::TonePeriod, pt)
{
}

CPSGBase::CPSGBase(UINT8 devid, CPort* pt, int regsize, UINT8 ch, int fsamp, int div, int off, FnumTableType type)
	: CSoundDevice(devid, ch, fsamp, div, off, type, pt, regsize)
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
	if (voice->ID & 0x00004000) {
		// Low level performance
	}
	else {
		if (keyon) {
			UpdateVoice(ch);
			if (!(voice->AL & 0x4)) {
				egattr[ch].Start(&voice->op[0]);
			}
		}
		else {
			egattr[ch].Release();
		}
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
CSSG::CSSG(CPort* pt, int fsamp) : CPSGBase(DEVICE_SSG, pt, 0x20, 3, fsamp)
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
	UINT16 etp = fnum->fnum >> (oct + 3);
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
CDCSG::CDCSG(CPort* pt, int fsamp)
	: CPSGBase(DEVICE_DCSG, pt, 0x10, 4, fsamp, 2, -576, FnumTableType::TonePeriod)
	, prevnoise(0)
{
	port->write(0, 0x80);
	port->write(0, 0x00);
	port->write(0, 0x9f);
	port->write(0, 0xa0);
	port->write(0, 0x00);
	port->write(0, 0xbf);
	port->write(0, 0xc0);
	port->write(0, 0x00);
	port->write(0, 0xdf);
	port->write(0, 0xe0);
	port->write(0, 0xff);
	GetChAttribute(3)->OutOfDVA();	// Noise ch should always be assigned manually
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
		UINT16 etp = fnum->fnum >> (oct + 3);
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
	case 2:
		break;
	case 3:
		if (attr->GetVoice()->AL == 1) {
			prevnoise = 0xe0 | ((attr->GetVoice()->FB & 1) << 2) | (attr->GetVoice()->NFQ & 0x3);
			port->writeRaw(0, prevnoise);
		}
		break;
	}
}

UINT8 CDCSG::QueryCh(CMidiCh* parent, FMVOICE* voice, int mode)
{
	UINT8 ret = 0xff;
	if (voice && voice->AL == 1) {
		ret = 3;
	}
	else {
		ret = CSoundDevice::QueryCh(parent, voice, mode);
	}
	return ret;
}

//-------------------------------

CEPSG::CEPSG(CPort* pt, int fsamp) : CPSGBase(DEVICE_EPSG, pt, 0x20, 3, fsamp), prevmix(0x3f)
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
	UINT16 etp = fnum->fnum >> (oct + 2);
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
CDSG::CDSG(CPort* pt, int fsamp) : CSoundDevice(DEVICE_DSG, 4, fsamp, 2, -576, FnumTableType::TonePeriod, pt, 0x80)
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
		port->write(0, 0x80 | addr);
		port->write(0, data);
	}
}

//-------------------------------
CSCCBase::CSCCBase(CPort* pt, int fsamp, const REGMAP& map)
	: CPSGBase(map.device, new COffsetPort(pt, map.offset), 0x100, 5, fsamp), regmap(map)
{
	ops = 1;
	pt->writeRaw(map.config, map.init);
	pt->writeRaw(map.banksel, map.bank);
	SetReg(map.enable, 0x1f, 1);
}

void CSCCBase::UpdateVolExp(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	if (!(attr->GetVoice()->op[0].EGT & 0x8)) {
		UINT8 evol = attr->GetEffectiveLevel();
		SINT16 lev = SINT16(lfoTL[ch]) - 64 + egattr[ch].GetValue();
		lev = (lev < 0) ? 0 : lev;
		lev = (lev > 127) ? 127 : lev;
		evol = 15 - Linear2dB(CalcLinearLevel(evol, 127 - UINT8(lev)), RANGE48DB, STEP300DB, 4);
		SetReg(regmap.amplitude + ch, evol & 0xf, 1);
	}
}

void CSCCBase::UpdateFreq(UINT8 ch, const FNUM* fnum)
{
	fnum = fnum ? fnum : GetChAttribute(ch)->GetLastFnumber();
	UINT8 oct = fnum->block;
	UINT16 etp = fnum->fnum >> (oct + 2);
	SetReg(ch * 2 + regmap.frequency, UINT8(etp & 0xff), 1);
	SetReg(ch * 2 + regmap.frequency, UINT8(etp >> 8), 1);
}

void CSCCBase::UpdateVoice(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	lfoTL[ch] = attr->baseTL[0] = 64;
	SCCWAVE wave;
	CFITOM::GetInstance()->GetWaveform(&wave, voice->op[0].WS);
	for (int i = 0; i < 32; i++) {
		int idx = (i + voice->FB) & 31;
		SetReg((ch * 32) + i + regmap.waveform, wave.wave[idx], 1);
	}
}

void CSCCBase::UpdateTL(UINT8 ch, UINT8 op, UINT8 lev)
{
	FMVOICE* voice = GetChAttribute(ch)->GetVoice();
	switch (op) {
	case 0:// Amplitude LFO
	{
		lfoTL[ch] = lev;
	}
	break;
	}
}

CSCC::CSCC(CPort* pt, int fsamp) : CSCCBase(pt, fsamp,
{ DEVICE_SCC, 0x00, 0x80, 0x8a, 0x8f, 0xc0, 0x00, 0x3f, 0x9800, 0xbffe, 0x9000 })
{
	GetChAttribute(4)->OutOfDVA();	// 5th ch should always be assigned manually
}

void CSCC::UpdateVoice(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	lfoTL[ch] = attr->baseTL[0] = 64;
	SCCWAVE wave;
	if (ch < 4) {
		CFITOM::GetInstance()->GetWaveform(&wave, voice->op[0].WS);
		for (int i = 0; i < 32; i++) {
			int idx = (i + voice->FB) & 31;
			SetReg((ch * 32) + i, wave.wave[idx], 1);
		}
	}
}

//-------------------------------
CSCCP::CSCCP(CPort* pt, int fsamp) : CSCCBase(pt, fsamp,
{ DEVICE_SCCP, 0x00, 0xa0, 0xaa, 0xaf, 0xc0, 0x20, 0x80, 0xb800, 0xbffe, 0xb000 })
{
}

//-------------------------------
CSAA::CSAA(CPort* pt, int fsamp) : CPSGBase(DEVICE_SAA, pt, 0x20, 6, fsamp, 1024*256, -640, FnumTableType::saa)
{
	ops = 1;
}

void CSAA::UpdateVolExp(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	if (!(attr->GetVoice()->op[0].EGT & 0x8)) {
		UINT8 evol = attr->GetEffectiveLevel();
		SINT16 lev = SINT16(lfoTL[ch]) - 64 + egattr[ch].GetValue();
		lev = (lev < 0) ? 0 : lev;
		lev = (lev > 127) ? 127 : lev;
		evol = 15 - Linear2dB(CalcLinearLevel(evol, 127 - UINT8(lev)), RANGE48DB, STEP300DB, 4);
		SetReg(0xaa + ch, evol & 0xf, 1);
	}
}

void CSAA::UpdateFreq(UINT8 ch, const FNUM* fnum)
{
	fnum = fnum ? fnum : GetChAttribute(ch)->GetLastFnumber();
	UINT8 oct = fnum->block;
	UINT16 etp = fnum->fnum >> (oct + 2);
	SetReg(ch * 2 + 0xa0, UINT8(etp & 0xff), 1);
	SetReg(ch * 2 + 0xa1, UINT8(etp >> 8), 1);
}

void CSAA::UpdateVoice(UINT8 ch)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	lfoTL[ch] = attr->baseTL[0] = 64;
	SCCWAVE wave;
	CFITOM::GetInstance()->GetWaveform(&wave, voice->op[0].WS);
	for (int i = 0; i < 32; i++) {
		int idx = (i + voice->FB) & 31;
		SetReg((ch * 32) + i, wave.wave[idx], 1);
	}
}

void CSAA::UpdateTL(UINT8 ch, UINT8 op, UINT8 lev)
{
	FMVOICE* voice = GetChAttribute(ch)->GetVoice();
	switch (op) {
	case 0:// Amplitude LFO
	{
		lfoTL[ch] = lev;
	}
	break;
	}
}
