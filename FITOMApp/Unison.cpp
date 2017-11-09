#include "STDAFX.H"
#include "MultiDev.h"

CUnison::CUnison(CSoundDevice* chip1, CSoundDevice* chip2)
	: CMultiDevice(chip1, chip2)
{
	chip2->OverrideAttribute(chip1->GetChAttribute(0));
}

void CUnison::UpdateFnumber(UINT8 ch, int update)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->UpdateFnumber(ch, update);
	}
}

void CUnison::UpdateVolExp(UINT8 ch)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->UpdateVolExp(ch);
	}
}

void CUnison::UpdateVoice(UINT8 ch)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->UpdateVoice(ch);
	}
}

void CUnison::UpdateFreq(UINT8 ch, const FNUM* fnum)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->UpdateFreq(ch, fnum);
	}
}

void CUnison::UpdateTL(UINT8 ch, UINT8 op, UINT8 lev)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->UpdateTL(ch, op, lev);
	}
}

void CUnison::UpdateKey(UINT8 ch, UINT8 keyon)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->UpdateKey(ch, keyon);
	}
}

void CUnison::UpdatePanpot(UINT8 ch)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->UpdatePanpot(ch);
	}
}

void CUnison::UpdateSustain(UINT8 ch)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->UpdateSustain(ch);
	}
}

void CUnison::SetReg(UINT16 reg, UINT8 data, UINT8 v)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->SetReg(reg, data, v);
	}
}

UINT8 CUnison::GetReg(UINT16 reg, UINT8 v)
{
	return chips[0] ? chips[0]->GetReg(reg, v) : 0;
}

void CUnison::PollingCallBack()
{
	for (int i=0; i<numchips; i++) {
		chips[i]->PollingCallBack();
	}
}

void CUnison::EnableCh(UINT8 ch, UINT8 ena)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->EnableCh(ch, ena);
	}
}

UINT8 CUnison::AllocCh(CMidiCh* parent, FMVOICE* voice)
{
	UINT8 ret = 0xff;
	if (numchips) {
		ret = chips[0]->AllocCh(parent, voice);
		chips[1]->Assign(ret, parent, voice);
	}
	return ret;
}

UINT8 CUnison::Assign(UINT8 ch, CMidiCh* parent, FMVOICE* voice)
{
	UINT8 ret = 0xff;
	for (int i=0; i<numchips; i++) {
		ret = chips[i]->Assign(ch, parent, voice);
	}
	return ret;
}

UINT8 CUnison::QueryCh(CMidiCh* parent, FMVOICE* voice, int mode)
{
	UINT8 ret = 0xff;
	if (numchips) {
		ret = chips[0]->QueryCh(parent, voice, mode);
	}
	return ret;
}

void CUnison::ReleaseCh(UINT8 ch)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->ReleaseCh(ch);
	}
}

UINT8 CUnison::GetChs()
{
	UINT8 ret = 0;
	if (numchips) {
		ret = chips[0]->GetChs();
	}
	return ret;
}

UINT8 CUnison::GetAvailable()
{
	UINT8 ret = 0;
	if (numchips) {
		ret = chips[0]->GetAvailable();
	}
	return ret;
}

UINT8 CUnison::GetAssignable()
{
	UINT8 ret = 0;
	if (numchips) {
		ret = chips[0]->GetAssignable();
	}
	return ret;
}

void CUnison::Reset()
{
	for (int i=0; i<numchips; i++) {
		chips[i]->Reset();
	}
}

void CUnison::SetSustain(UINT8 ch, UINT8 sus, int update)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->SetSustain(ch, sus, update);
	}
}

void CUnison::SetNoteFine(UINT8 ch, UINT8 note, SINT16 fine, int update)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->SetNoteFine(ch, note, fine, update);
	}
}

void CUnison::SetVoice(UINT8 ch, FMVOICE* voice, int update)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->SetVoice(ch, voice, update);
	}
}

void CUnison::ResetVoice(CMidiCh* pch, FMVOICE* voice, int update)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->ResetVoice(pch, voice, update);
	}
}

void CUnison::SetVolume(UINT8 ch, UINT8 vol, int update)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->SetVolume(ch, vol, update);
	}
}

void CUnison::SetVelocity(UINT8 ch, UINT8 vel, int update)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->SetVelocity(ch, vel, update);
	}
}

void CUnison::SetExpress(UINT8 ch, UINT8 exp, int update)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->SetExpress(ch, exp, update);
	}
}

void CUnison::SetPanpot(UINT8 ch, UINT8 pan, int update)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->SetPanpot(ch, pan, update);
	}
}

void CUnison::SetMasterVolume(UINT8 vol, int update)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->SetMasterVolume(vol, update);
	}
}

void CUnison::SetLFOParam(FMVOICE* voice, int update)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->SetLFOParam(voice, update);
	}
}

void CUnison::SetLFOMode(UINT8 ch, UINT8 mode, int update)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->SetLFOMode(ch, mode, update);
	}
}

void CUnison::EnableDevPM(UINT8 ch, UINT8 on)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->EnableDevPM(ch, on);
	}
}

void CUnison::EnableDevAM(UINT8 ch, UINT8 on)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->EnableDevAM(ch, on);
	}
}

void CUnison::SetDevPMDepth(UINT8 ch, UINT8 dep)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->SetDevPMDepth(ch, dep);
	}
}

void CUnison::SetDevAMDepth(UINT8 ch, UINT8 dep)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->SetDevAMDepth(ch, dep);
	}
}

void CUnison::SetDevPMRate(UINT8 ch, UINT8 rate)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->SetDevPMRate(ch, rate);
	}
}

void CUnison::SetDevAMRate(UINT8 ch, UINT8 rate)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->SetDevAMRate(ch, rate);
	}
}

UINT32 CUnison::GetCurrentVoiceID(UINT8 ch) const
{
	UINT32 ret = 0xffffffffUL;
	if (numchips) {
		ret = chips[0]->GetCurrentVoiceID(ch);
	}
	return ret;
}

FMVOICE* CUnison::GetCurrentVoice(UINT8 ch) const
{
	FMVOICE* ret = 0;
	if (numchips) {
		ret = chips[0]->GetCurrentVoice(ch);
	}
	return ret;
}

UINT8 CUnison::GetCurrentNote(UINT8 ch) const
{
	UINT8 ret = 0xff;
	if (numchips) {
		ret = chips[0]->GetCurrentNote(ch);
	}
	return ret;
}

ISoundDevice::FNUM CUnison::GetCurrentFnum(UINT8 ch) const
{
	FNUM ret;
	if (numchips) {
		ret = chips[0]->GetCurrentFnum(ch);
	}
	return ret;
}

void CUnison::GetDescriptor(char* str, size_t len)
{
	if (numchips) {
		chips[0]->GetDescriptor(str, len);
	}
}

void CUnison::NoteOn(UINT8 ch, UINT8 vel)
{
	for (int i=0; i<numchips; i++) {
		chips[0]->NoteOn(ch, vel);
	}
}

void CUnison::NoteOff(UINT8 ch)
{
	for (int i=0; i<numchips; i++) {
		chips[0]->NoteOff(ch);
	}
}
