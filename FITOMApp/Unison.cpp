#include "STDAFX.H"
#include "MultiDev.h"

CUnison::CUnison(CSoundDevice* chip1, CSoundDevice* chip2)
	: CMultiDevice(chip1, chip2)
{
	chip2->OverrideAttribute(chip1->GetChAttribute(0));
}

void CUnison::UpdateFnumber(uint8_t ch, int update)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->UpdateFnumber(ch, update);
	}
}

void CUnison::UpdateVolExp(uint8_t ch)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->UpdateVolExp(ch);
	}
}

void CUnison::UpdateVoice(uint8_t ch)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->UpdateVoice(ch);
	}
}

void CUnison::UpdateFreq(uint8_t ch, const FNUM* fnum)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->UpdateFreq(ch, fnum);
	}
}

void CUnison::UpdateTL(uint8_t ch, uint8_t op, uint8_t lev)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->UpdateTL(ch, op, lev);
	}
}

void CUnison::UpdateKey(uint8_t ch, uint8_t keyon)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->UpdateKey(ch, keyon);
	}
}

void CUnison::UpdatePanpot(uint8_t ch)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->UpdatePanpot(ch);
	}
}

void CUnison::UpdateSustain(uint8_t ch)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->UpdateSustain(ch);
	}
}

void CUnison::SetReg(uint16_t reg, uint8_t data, uint8_t v)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->SetReg(reg, data, v);
	}
}

uint8_t CUnison::GetReg(uint16_t reg, uint8_t v)
{
	return chips[0] ? chips[0]->GetReg(reg, v) : 0;
}

void CUnison::PollingCallBack()
{
	for (int i=0; i<numchips; i++) {
		chips[i]->PollingCallBack();
	}
}

void CUnison::EnableCh(uint8_t ch, uint8_t ena)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->EnableCh(ch, ena);
	}
}

uint8_t CUnison::AllocCh(CMidiCh* parent, FMVOICE* voice)
{
	uint8_t ret = 0xff;
	if (numchips) {
		ret = chips[0]->AllocCh(parent, voice);
		chips[1]->Assign(ret, parent, voice);
	}
	return ret;
}

uint8_t CUnison::Assign(uint8_t ch, CMidiCh* parent, FMVOICE* voice)
{
	uint8_t ret = 0xff;
	for (int i=0; i<numchips; i++) {
		ret = chips[i]->Assign(ch, parent, voice);
	}
	return ret;
}

uint8_t CUnison::QueryCh(CMidiCh* parent, FMVOICE* voice, int mode)
{
	uint8_t ret = 0xff;
	if (numchips) {
		ret = chips[0]->QueryCh(parent, voice, mode);
	}
	return ret;
}

void CUnison::ReleaseCh(uint8_t ch)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->ReleaseCh(ch);
	}
}

uint8_t CUnison::GetChs()
{
	uint8_t ret = 0;
	if (numchips) {
		ret = chips[0]->GetChs();
	}
	return ret;
}

uint8_t CUnison::GetAvailable()
{
	uint8_t ret = 0;
	if (numchips) {
		ret = chips[0]->GetAvailable();
	}
	return ret;
}

uint8_t CUnison::GetAssignable()
{
	uint8_t ret = 0;
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

void CUnison::SetSustain(uint8_t ch, uint8_t sus, int update)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->SetSustain(ch, sus, update);
	}
}

void CUnison::SetNoteFine(uint8_t ch, uint8_t note, int16_t fine, int update)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->SetNoteFine(ch, note, fine, update);
	}
}

void CUnison::SetVoice(uint8_t ch, FMVOICE* voice, int update)
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

void CUnison::SetVolume(uint8_t ch, uint8_t vol, int update)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->SetVolume(ch, vol, update);
	}
}

void CUnison::SetVelocity(uint8_t ch, uint8_t vel, int update)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->SetVelocity(ch, vel, update);
	}
}

void CUnison::SetExpress(uint8_t ch, uint8_t exp, int update)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->SetExpress(ch, exp, update);
	}
}

void CUnison::SetPanpot(uint8_t ch, uint8_t pan, int update)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->SetPanpot(ch, pan, update);
	}
}

void CUnison::SetMasterVolume(uint8_t vol, int update)
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

void CUnison::SetLFOMode(uint8_t ch, uint8_t mode, int update)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->SetLFOMode(ch, mode, update);
	}
}

void CUnison::EnableDevPM(uint8_t ch, uint8_t on)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->EnableDevPM(ch, on);
	}
}

void CUnison::EnableDevAM(uint8_t ch, uint8_t on)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->EnableDevAM(ch, on);
	}
}

void CUnison::SetDevPMDepth(uint8_t ch, uint8_t dep)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->SetDevPMDepth(ch, dep);
	}
}

void CUnison::SetDevAMDepth(uint8_t ch, uint8_t dep)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->SetDevAMDepth(ch, dep);
	}
}

void CUnison::SetDevPMRate(uint8_t ch, uint8_t rate)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->SetDevPMRate(ch, rate);
	}
}

void CUnison::SetDevAMRate(uint8_t ch, uint8_t rate)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->SetDevAMRate(ch, rate);
	}
}

uint32_t CUnison::GetCurrentVoiceID(uint8_t ch) const
{
	uint32_t ret = 0xffffffffUL;
	if (numchips) {
		ret = chips[0]->GetCurrentVoiceID(ch);
	}
	return ret;
}

FMVOICE* CUnison::GetCurrentVoice(uint8_t ch) const
{
	FMVOICE* ret = 0;
	if (numchips) {
		ret = chips[0]->GetCurrentVoice(ch);
	}
	return ret;
}

uint8_t CUnison::GetCurrentNote(uint8_t ch) const
{
	uint8_t ret = 0xff;
	if (numchips) {
		ret = chips[0]->GetCurrentNote(ch);
	}
	return ret;
}

ISoundDevice::FNUM CUnison::GetCurrentFnum(uint8_t ch) const
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

void CUnison::NoteOn(uint8_t ch, uint8_t vel)
{
	for (int i=0; i<numchips; i++) {
		chips[0]->NoteOn(ch, vel);
	}
}

void CUnison::NoteOff(uint8_t ch)
{
	for (int i=0; i<numchips; i++) {
		chips[0]->NoteOff(ch);
	}
}
