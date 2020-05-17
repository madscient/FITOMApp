#include "stdafx.h"
#include "Port.h"
#include "SoundDev.h"
#include "MIDI.h"
#include "FITOM.h"
#include "SCCIWrapper.h"
#include "Fnum.h"

namespace ROM {
extern const uint8_t VolCurveLin[];
extern const uint8_t VolCurveInv[];
};

ISoundDevice::FNUM::FNUM() : block(0), fnum(0)
{
}

ISoundDevice::FNUM::FNUM(uint8_t bl, uint16_t fn) : block(bl), fnum(fn)
{
}

ISoundDevice::CHATTR::CHATTR() : 
	status(CHATTR::EMPTY), velocity(255), dva(true),
	lastfnum(FNUM(0, 0)), lastvel(0), lastnote(255), finefreq(0), count(0), parent(0), device(0)
{
	memset(&voice, 0, sizeof(FMVOICE));
	voice.ID = 0xffffffff;
}

ISoundDevice::LFORESOURCE::LFORESOURCE() : used(0), parent(0), amrate(0), amdepth(0), pmrate(0), pmdepth(0)
{
}

void CSoundDevice::CHATTR::Init()
{
	status = IsEnable() ? CHATTR::EMPTY : CHATTR::DISABLED;
	lastnote = 255;
	lastvel = 0;
	lastfnum = FNUM(0, 0);
	velocity = 255;
	count = 0;
	device = 0;
	memset(&voice, 0, sizeof(FMVOICE));
	voice.ID = 0xffffffff;
	dva = dva ? true : false;
}

void CSoundDevice::CHATTR::Assign(CMidiCh* parch)
{
	if (IsEnable()) {
		status = ASSIGNED;
		parent = parch;
		noteon = 0;
		chlfo.Stop();
		for (int i = 0; i < 4; i++) {
			oplfo[i].Stop();
			baseTL[i] = 64;
		}
	}
}

void CSoundDevice::CHATTR::Run()
{
	if (IsEnable()) {
		status = RUNNING;
		noteon = 1;
	}
}

void CSoundDevice::CHATTR::Stop()
{
	if (IsRunning()) {
		status = ASSIGNED;
	}
}

void CSoundDevice::CHATTR::Release(int forgetparent)
{
	if (IsEnable()) {
		status = EMPTY;
		noteon = 0;
		if (forgetparent) {
			parent = 0;
		}
	}
}
void CSoundDevice::CHATTR::Enable(int ena)
{
	if (ena && !IsEnable()) {
		status = EMPTY;
	} else {
		status = DISABLED;
	}
}

bool CSoundDevice::CHATTR::SetVoice(FMVOICE* vc)
{
	if (memcmp(&voice, vc, sizeof(FMVOICE)) == 0) {
		return FALSE;
	}
	voice = *vc;
	return TRUE;
}

void CSoundDevice::CHATTR::SetVoiceID(uint32_t vcid)
{
	voice.ID = vcid;
}

int16_t CSoundDevice::CHATTR::GetChLFOValue()
{
	int16_t ret = 0;
	if (chlfo.GetStatus() != CSoundDevice::CLFOControl::LFO_NONE) {
		int16_t lev = int16_t(chlfo.GetValue()); // <-- 0~127 absolute level
		if (lev) {
			int32_t val = int16_t(GetLFOWave(voice.LWF, voice.LFO, chlfo.GetCount())); // <-- -120 ~ +120
			val = (val > 120) ? 120 : val;
			val = (val < -120) ? -120 : val;
			int32_t dep = ((uint16_t(voice.LDM&0x7f)<<7)|(voice.LDL&0x7f));
			if (dep > 8191) { dep -= 16384; } // <-- 0~16383 as -8191~+8191
			val = val * lev / 128;
			val = val * dep / 120;
			val = (val > 8191) ? 8191 : val;
			val = (val < -8192) ? -8192 : val;
			ret = int16_t(val);
		}
	}
	return ret;
}

int32_t CSoundDevice::CHATTR::GetNoteIndex(int16_t offset)
{
	return lastnote * 64 + finefreq + GetChLFOValue() + offset;
}

void CSoundDevice::CHATTR::UpdateFnumber(ISoundDevice::FNUM* fnum)
{
	if (fnum) {
		lastfnum = *fnum;
	}
}

uint8_t CSoundDevice::CHATTR::GetEffectiveLevel()
{
	return ::CalcVolExpVel(GetVolume(), GetExpress(), velocity);
}

void CSoundDevice::CHATTR::SetNoteFine(uint8_t note, int16_t fine)
{
	lastnote = note;
	finefreq = fine;
}

uint8_t CSoundDevice::CHATTR::GetVolume() { return (parent != NULL) ? parent->GetTrackVolume() : 0; }
uint8_t CSoundDevice::CHATTR::GetExpress() { return (parent != NULL) ? parent->GetExpress() : 0; }
uint8_t CSoundDevice::CHATTR::GetPanpot() { return (parent != NULL) ? parent->GetPanpot() : 64; }


/*----------*/
CSoundDevice::CSoundDevice(uint8_t devid, uint8_t maxchs, int fsamp, int devide, int offset, FnumTableType ftype, CPort* pt, int regsize) :
device(devid), chs(maxchs), port(pt), ops(4), prior_ch(0), NoteOffset(60), MasterTune(0), regbak(0)
{
	if (regsize) {
		regbak = new BYTE[regsize];
		memset(regbak, 0, regsize);
	}
	if (maxchs) {
		chattr = new CHATTR[int(maxchs)];
	} else {
		chattr = 0;
	}
	Fnum = theFnum.GetTable(ftype, fsamp, devide, offset);
}

CSoundDevice::~CSoundDevice()
{
	if (chs)
	{
		delete[] chattr;
		chattr = 0;
	}
	if (regbak) {
		delete[] regbak;
	}
}

int CSoundDevice::GetLocation()
{
	if (port) {
		return port->GetPanpot();
	}
	return 0;
}

void CSoundDevice::OverrideAttribute(ISoundDevice::CHATTR* attr)
{
	delete[] chattr;
	chattr = attr;
}

ISoundDevice::CHATTR* CSoundDevice::GetChAttribute(uint8_t ch) const
{
	return (ch < chs) ? &chattr[ch] : 0;
}

void CSoundDevice::Reset()
{
	//if (port) port->reset();
	for (int i=0; i<chs; i++) {
		Damp(i);
		CHATTR* attr = GetChAttribute(i);
		if (attr) {
			attr->Init();
		}
	}
}

void CSoundDevice::SetDevice(uint8_t devid)
{
	device = devid;
}

void CSoundDevice::SetReg(uint16_t reg, uint8_t data, uint8_t v)
{
	if (!v && regbak) {
		v = (regbak[reg] != data);
	}
	if (regbak) {
		regbak[reg] = data;
	}
	(v && port) ? port->write(reg, data) : 0;
}

uint8_t CSoundDevice::GetReg(uint16_t reg, uint8_t v)
{
	uint8_t ret = 0;
	ret = v ? (port ? port->read(reg) : 0) : (regbak ? regbak[reg] : 0);
	return ret;
}

void CSoundDevice::SetSustain(uint8_t ch, uint8_t sus, int update)
{
	if (ch < chs) {
		if (update) {
			UpdateSustain(ch);
		}
	}
}

void CSoundDevice::SetVoice(uint8_t ch, FMVOICE* voice, int update)
{
	if (ch < chs) {
		CHATTR* attr = GetChAttribute(ch);
		bool changed = attr->SetVoice(voice);
		if (changed) {
			for (int i = 0; i < ops; i++) {
				attr->baseTL[i] = voice->op[i].TL;
			}
			UpdateVoice(ch);
		}
	}
}

void CSoundDevice::ResetVoice(CMidiCh* pch, FMVOICE* voice, int update)
{
	for (int i=0; i<chs; i++) {
		if (GetChAttribute(i)->GetParent() == pch || !pch) {
			if (voice) {
				SetVoice(i, voice, -1);
			} else {
				GetChAttribute(i)->SetVoiceID(0xffffffffL);
			}
		}
	}
}

void CSoundDevice::SetVolume(uint8_t ch, uint8_t vol, int update)
{
	if (ch < chs) {
		if (GetChAttribute(ch)->GetVolume() != vol) {
			if (update) {
				UpdateVolExp(ch);
			}
		}
	}
}

void CSoundDevice::SetExpress(uint8_t ch, uint8_t exp, int update)
{
	if (ch < chs) {
		if (GetChAttribute(ch)->GetExpress() != exp) {
			if (update) {
				UpdateVolExp(ch);
			}
		}
	}
}

void CSoundDevice::SetPanpot(uint8_t ch, uint8_t pan, int update)
{
	if (ch < chs)
	{
		int panpot = pan - 64;
		if (GetChAttribute(ch)->GetPanpot() != panpot) {
			if (update) {
				UpdatePanpot(ch);
			}
		}
	}
}

void CSoundDevice::SetMasterVolume(uint8_t vol, int update)
{
	if (volume != vol) {
		volume = vol;
		for (int i=0; update && i<chs; i++)
		{
			if (GetChAttribute(i)->IsEnable()) {
				UpdateVolExp((uint8_t)i);
			}
		}
	}
}

uint8_t CSoundDevice::AllocCh(CMidiCh* parent, FMVOICE* voice)
{
	volatile uint8_t ret = 0xff;
	ret = QueryCh(parent, voice, 1);
	if (ret == 0xff) {
		ret = QueryCh(NULL, voice, 1);
	}
	if (ret == 0xff) {
		ret = QueryCh(NULL, NULL, 1);
	}
	if (ret == 0xff) {
		DWORD count = 0;
		int cand = 0xff;
		for (int i = 0; i < chs; i++) {
			CHATTR* attr = GetChAttribute(i);
			if (attr->IsAutoAssignable() && attr->IsEnable()) {
				if (count < attr->noteon) {
					cand = i;
					count = attr->noteon;
				}
			}
		}
		ret = cand;
		if (ret == 0xff) {
			ret = QueryCh(NULL, NULL, 0);
		}
#if defined(_DEBUG) || defined(_CONSOLE)
		fprintf(stderr, _T("AllocCh:%i:partial out!!(%i)\n"), ret, count);
#endif
	}
	if (ret != 0xff) {
		Assign(ret, parent, voice);
	}
	return (ret);
}

uint8_t CSoundDevice::Assign(uint8_t ch, CMidiCh* parent, FMVOICE* voice)
{
	CHATTR* attr = GetChAttribute(ch);
	if (ch < chs) {
		if (attr->IsRunning()) {	//partial out!
			if (attr->GetParent()) {
				if (!attr->GetParent()->IsRhythm()) {
					attr->GetParent()->NoteOff(attr->GetLastNote());
				} else {
					((CRhythmCh*)(attr->GetParent()))->NoteOff(this, ch);
				}
			} else {
				NoteOff(ch);	// should never come here...
			}
		}
		if (attr->IsEnable()) {
			attr->Assign(parent);
			SetVoice(ch, voice);
			return ch;
		} else {
			return 0xff;
		}
	} else {
		return 0xff;
	}
}

uint8_t CSoundDevice::QueryCh(CMidiCh* parent, FMVOICE* voice, int mode)
{
	volatile uint8_t ret = 0xff;
	uint8_t tmp = 0xff;
	CHATTR* attr = 0;
	if (parent && voice) {
		tmp = prior_ch;
		for (int i=0; i<chs; i++) {
			attr = GetChAttribute(tmp);
			if (attr && attr->IsAutoAssignable() && (mode ? attr->IsAvailable() : attr->IsEnable())
				&& memcmp(voice, attr->GetVoice(), sizeof(FMVOICE)) == 0 && attr->GetParent() == parent) {
				ret = tmp;
#if defined(_DEBUG) || defined(_CONSOLE)
				fprintf(stderr, _T("QueryCh:%i:mached parent and voice\n"), ret);
#endif
				break;
			}
			tmp = (tmp + 1) % chs;
		}
	} else if (voice) {
		tmp = prior_ch;
		for (int i = 0; i<chs; i++) {
			attr = GetChAttribute(tmp);
			if (attr && attr->IsAutoAssignable() && (mode ? attr->IsAvailable() : attr->IsEnable())
				&& attr->GetVoiceID() == voice->ID) {
				ret = tmp;
#if defined(_DEBUG) || defined(_CONSOLE)
				fprintf(stderr, _T("QueryCh:%i:mached voice\n"), ret);
#endif
				break;
			}
			tmp = (tmp + 1) % chs;
		}
	} else {
		tmp = prior_ch;
		for (int i=0; i<chs; i++) {
			attr = GetChAttribute(tmp);
			if (attr && attr->IsAutoAssignable() && (mode ? attr->IsAvailable() : 1) && attr->IsEnable()) {
				ret = tmp;
#if defined(_DEBUG) || defined(_CONSOLE)
				fprintf(stderr, _T("QueryCh:%i:new ch\n"), ret);
#endif
				break;
			}
			tmp = (tmp + 1) % chs;
		}
	}
	if (ret != 0xff) {
		prior_ch = (prior_ch + 1) % chs;
	}
	return ret;
}

void CSoundDevice::ReleaseCh(uint8_t ch)
{
	if (ch < chs)
	{
		if (GetChAttribute(ch)->IsEnable()) {
			GetChAttribute(ch)->Release(0);
		} else {
			int oh_my_god = 1;
		}
		//chattr[ch].parent = 0;
	}
}

void CSoundDevice::EnableCh(uint8_t ch, uint8_t ena)
{
	if (ch < chs)
	{
		GetChAttribute(ch)->Enable(ena);
	}
}

void CSoundDevice::SetVelocity(uint8_t ch, uint8_t vel, int update)
{
	if (ch < chs && vel < 128) {
		GetChAttribute(ch)->velocity = vel;
		if (update) {
			UpdateVolExp(ch);
		}
	}
}

void CSoundDevice::SetNoteFine(uint8_t ch, uint8_t note, int16_t fine, int update)
{
	if (ch < chs) {
		GetChAttribute(ch)->SetNoteFine(note, fine);
		UpdateFnumber(ch, update);
	}
}

uint8_t CSoundDevice::GetAvailable()
{
	uint8_t ret = 0;
	for (int i=0; i<chs; i++) {
		if (GetChAttribute(i)->IsEnable()) { ret++; }
	}
	return ret;
}

uint8_t CSoundDevice::GetAssignable()
{
	uint8_t ret = 0;
	for (int i=0; i<chs; i++) {
		if (GetChAttribute(i)->IsAvailable()) { ret++; }
	}
	return ret;
}

uint32_t CSoundDevice::GetCurrentVoiceID(uint8_t ch) const
{
	uint32_t ret = 0;
	if (ch < chs) {
		ret = GetChAttribute(ch)->GetVoiceID();
	}
	return ret;
}

FMVOICE* CSoundDevice::GetCurrentVoice(uint8_t ch) const
{
	FMVOICE* ret = 0;
	if (ch < chs) {
		ret = GetChAttribute(ch)->GetVoice();
	}
	return ret;
}

uint8_t CSoundDevice::GetCurrentNote(uint8_t ch) const
{
	uint8_t ret = 255;
	if (ch < chs && GetChAttribute(ch)->IsRunning()) {
		ret = GetChAttribute(ch)->GetLastNote();
	}
	return ret;
}

ISoundDevice::FNUM CSoundDevice::GetCurrentFnum(uint8_t ch) const
{
	FNUM ret;
	if (ch < chs && GetChAttribute(ch)->IsRunning()) {
		ret = *GetChAttribute(ch)->GetLastFnumber();
	}
	return ret;
}

void CSoundDevice::GetDescriptor(TCHAR* str, size_t len)
{
	StringCchPrintf(str, len, _T("%02X:%s (%ich)"), device, CFITOM::GetDeviceNameFromID(device), chs);
}

void CSoundDevice::PollingCallBack()
{
}

void CSoundDevice::TimerCallBack(uint32_t tick)
{
	int tflag = (tick&1);
	if (/*tflag == 0*/1) {
		for (int i=0; i<chs; i++) {
			CHATTR* attr = GetChAttribute(i);
			if (attr->IsRunning()) {
				attr->noteon++;
				if (attr->chlfo.Update()) {
					UpdateFnumber(i);
				}
				for (int j=0; j<4; j++) {
					if (attr->oplfo[j].Update()) {
						UpdateOpLFO(i, j);
					}
				}
			}
		}
	}
}

ISoundDevice::FNUM CSoundDevice::GetFnumber(uint8_t ch, int16_t offset)
{
	FNUM ret;
	CHATTR* attr = GetChAttribute(ch);
	if (Fnum && attr && attr->GetLastNote() < 128) {
		int index = attr->GetNoteIndex(MasterTune + (NoteOffset * 64) + offset);
		int oct = (-NoteOffset / 12) - 1;

		// normalize
		if (index < 0) {
			int div = (-index) / 768;
			oct -= div + 1;
			index += (div + 1) * 768;
		}
		if (index >= 768) {
			int div = index / 768;
			index = index % 768;
			oct += div;
		}
#if 0
		while (index < 0) {
			oct--;
			index += 768;
		}
		while (index >= 768) {
			oct++;
			index -= 768;
		}
#endif
		assert(0 <= index && index < 768);
		ret.fnum = Fnum[index];
		if (oct < 0) {
			ret.fnum = (ret.fnum >> (-oct));
			oct = 0;
		}
		if (ret.fnum & 0x800) {
			ret.fnum >>= 1;
			oct++;
		}
		if (oct > 7) {
			ret.block = 7;
			ret.fnum = ((ret.fnum << (oct - 7)) | 1);
		}
		else {
			ret.block = uint8_t(oct);
		}
	}
	else if (attr) {
		ret.block = uint16_t(attr->GetLastFineFreq()) >> 12;
		ret.fnum = uint16_t(attr->GetLastFineFreq()) & 0xfff;
	}
	return ret;
}

void CSoundDevice::UpdateFnumber(uint8_t ch, int update)
{
	if (ch < chs) {
		FNUM fnum = GetFnumber(ch);
		GetChAttribute(ch)->UpdateFnumber(&fnum);
		if (update) {
			UpdateFreq(ch);
		}
	}
}

void CSoundDevice::UpdateOpLFO(uint8_t ch, uint8_t op)
{
	CHATTR* attr = GetChAttribute(ch);
	FMVOICE* voice = attr->GetVoice();
	int16_t lev = int16_t(attr->oplfo[op].GetValue());
	if (lev) {
		int16_t val = int16_t(GetLFOWave(voice->op[op].SLY, voice->op[op].SLF, attr->oplfo[op].GetCount()));
		int16_t dep = int16_t(voice->op[op].SLD);
		if (dep > 63) { dep -= 128; }
		val = val * lev / 128; 
		val = val * dep / 120;
		val += attr->baseTL[op];
		val = (val < 0) ? 0 : val;
		val = (val > 127) ? 127 : val;
		UpdateTL(ch, op, uint8_t(val));
	} else {
		UpdateTL(ch, op,  attr->baseTL[op]);
	}
}

void CSoundDevice::NoteOn(uint8_t ch, uint8_t vel)
{
	if (ch < chs) {
		CHATTR* attr = GetChAttribute(ch);
		FMVOICE* voice = attr->GetVoice();
		if (attr->IsEnable()) {
			/* ‚È‚ñ‚¾‚Á‚¯‚±‚êH
			if (voice->AL & 0x40) {
				UpdateKey(ch, 0);
			}
			*/
			SetVelocity(ch, vel);
			attr->Run();
			if (voice->LFR) {
				attr->chlfo.Start(voice->LFD, voice->LFR);
			}
			else {
				attr->chlfo.Stop();
			}
			for (int op=0; op<ops; op++) {
				if (voice->op[op].SLR) {
					attr->oplfo[op].Start(voice->op[op].SLY, voice->op[op].SLR);
				}
				else {
					attr->oplfo[op].Stop();
				}
			}
			UpdateKey(ch, 1);
		} else {
			int oh_my_god = 1;
		}
	}
}

void CSoundDevice::NoteOff(uint8_t ch)
{
	if (ch < chs) {
		CHATTR* attr = GetChAttribute(ch);
		if (attr) {
			attr->Release(0);
			if (0) {	//lfo should be continued in release phase
				attr->chlfo.Stop();
				for (int op = 0; op < ops; op++) {
					attr->oplfo[op].Stop();
				}
			}
			UpdateKey(ch, 0);
		} else {
			int oh_my_god = 1;
		}
	}
}

#if 0
CMidiCh* CSoundDevice::GetParent(uint8_t ch) const
{
	if (ch < chs) {
		CHATTR* attr = GetChAttribute(ch);
		if (attr) {
			return attr->parent;
		}
	}
	return NULL;
}

ISoundDevice::CHATTR::CHSTAT CSoundDevice::GetChStatus(uint8_t ch) const
{
	if (ch < chs) {
		CHATTR* attr = GetChAttribute(ch);
		if (attr) {
			return attr->status;
		}
	}
	return CHATTR::EMPTY;
}
#endif

CRhythmDevice::CRhythmDevice(CSoundDevice* parent, uint8_t devid, uint8_t maxch) :
	CSoundDevice(devid, maxch, 0, 0, 0, FnumTableType::none, 0, 0), pParent(parent)
{
	if (parent) {
		Fnum = parent->Fnum;
		port = parent->GetDevPort();
		NoteOffset = parent->NoteOffset;
		MasterTune = parent->MasterTune;
	}
}

CRhythmDevice::CRhythmDevice(CPort* pt, uint8_t devid, uint8_t maxch) :
	CSoundDevice(devid, maxch, 0, 0, 0, FnumTableType::none, 0, 0), pParent(NULL)
{
	port = pt;
}
