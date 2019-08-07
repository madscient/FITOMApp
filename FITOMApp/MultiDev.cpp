#include "STDAFX.H"
#include "MultiDev.h"

CMultiDevice::CMultiDevice(CSoundDevice* chip1, CSoundDevice* chip2)
	: CSoundDevice(0, 0, 0, 0, 0, FnumTableType::none, 0), numchips(0), chips(NULL)
{
	if (chip1 && chip2) {
		AddDevice(chip1);
		AddDevice(chip2);
		device = chip1->GetDevice();
	}
}

void CMultiDevice::Init()
{
	for (int i = 0; i<numchips; i++) {
		chips[i]->Init();
	}
}

void CMultiDevice::AddDevice(CSoundDevice* chip)
{
	CSoundDevicePtr* newchips = new CSoundDevicePtr[numchips+1];
	if (chips) {
		for (int i=0; i<numchips; i++) {
			newchips[i] = chips[i];
		}
		delete [] chips;
	}
	chips = newchips;
	newchips[numchips] = chip;
	numchips++;
}

void CMultiDevice::SetMasterVolume(uint8_t vol, int update)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->SetMasterVolume(vol, update);
	}
}

void CMultiDevice::ResetVoice(CMidiCh* pch, FMVOICE* voice, int update)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->ResetVoice(pch, voice, update);
	}
}

void CMultiDevice::TimerCallBack(uint32_t tick)
{
	for (int i=0; i<numchips; i++) {
		chips[i]->TimerCallBack(tick);
	}
}

void CMultiDevice::PollingCallBack()
{
	for (int i=0; i<numchips; i++) {
		chips[i]->PollingCallBack();
	}
}

void CMultiDevice::SetReg(uint16_t reg, uint8_t data, uint8_t v)
{
	if (!port) {
		chips[0]->GetDevPort()->write(reg, data);
	} else {
		CSoundDevice::SetReg(reg, data, v);
	}
}

uint8_t CMultiDevice::GetReg(uint16_t reg, uint8_t v)
{
	uint8_t ret = 0;
	if (!port) {
		ret = chips[0]->GetDevPort()->read(reg);
	} else {
		ret = CSoundDevice::GetReg(reg, v);
	}
	return ret;
}

void CMultiDevice::Reset()
{
	for (int i = 0; i < numchips; i++) {
		chips[i]->Reset();
	}
}
