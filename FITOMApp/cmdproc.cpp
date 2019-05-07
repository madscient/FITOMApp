#include "stdafx.h"
#include "FITOMdefine.h"
#include "FITOM.h"
#include "MIDIDEV.h"
#include "MIDI.h"
#include "ADPCM.h"

BOOL CFITOM::cmdSetVoice(TCHAR * cmd, TCHAR * result, int reslen)
{
	return 0;
}

BOOL CFITOM::cmdGetVoice(TCHAR * cmd, TCHAR * result, int reslen)
{
	return 0;
}

BOOL CFITOM::cmdGetMidiStatus(TCHAR * cmd, TCHAR * result, int reslen)
{
	return 0;
}

BOOL CFITOM::cmdGetDeviceStatus(TCHAR * cmd, TCHAR * result, int reslen)
{
	return 0;
}

BOOL CFITOM::cmdSendMidi(TCHAR * cmd, TCHAR * result, int reslen)
{
	return 0;
}

BOOL CFITOM::cmdGetPhysicalDevice(TCHAR * cmd, TCHAR * result, int reslen)
{
	return 0;
}

BOOL CFITOM::cmdGetLogicalDevice(TCHAR * cmd, TCHAR * result, int reslen)
{
	return 0;
}

BOOL CFITOM::cmdGetMidiDevice(TCHAR * cmd, TCHAR * result, int reslen)
{
	return 0;
}

BOOL CFITOM::cmdSetPcmVoice(TCHAR * cmd, TCHAR * result, int reslen)
{
	return 0;
}

BOOL CFITOM::cmdGetPcmVoice(TCHAR * cmd, TCHAR * result, int reslen)
{
	return 0;
}

BOOL CFITOM::cmdSetWaveForm(TCHAR * cmd, TCHAR * result, int reslen)
{
	return 0;
}

BOOL CFITOM::cmdGetWaveForm(TCHAR * cmd, TCHAR * result, int reslen)
{
	return 0;
}

BOOL CFITOM::cmdSetDeviceReg(TCHAR * cmd, TCHAR * result, int reslen)
{
	return 0;
}

BOOL CFITOM::cmdGetDeviceReg(TCHAR * cmd, TCHAR * result, int reslen)
{
	return 0;
}

int CFITOM::PollingCallBack()
{
	int i;
	int ret = 0;
	if (!timerprocessing && !pollprocessing) {
		pollprocessing = 1;
		for (i = 0; i < theConfig->GetMidiInputs(); i++) {
			ret += midi[i]->PollingCallBack();
		}
		for (int i = 0; i < theConfig->GetLogDevs(); i++) {
			CSoundDevice* pdev = theConfig->GetLogDeviceFromIndex(i);
			if (pdev) {
				pdev->PollingCallBack();
			}
		}
		pollprocessing = 0;
	}
	return ret;
}

void CFITOM::TimerCallBack(UINT32 tick)
{
	if (!timerprocessing) {
		timerprocessing = 1;
		int wait = 0;
		for (wait = 0; pollprocessing; wait++);
#ifdef _DEBUG
		DWORD tfrom = timeGetTime();
		if (wait) {
			//	DWORD delay = timeGetTime() - tfrom;
			//	fprintf(stderr, _T("%i: poller waited for %icounts (%ims)\n"), tick, wait, delay);
		}
#endif
		int i;
		for (i = 0; i < theConfig->GetMidiInputs(); i++) {
			midi[i]->TimerCallBack(tick);
		}
		for (int i = 0; i < theConfig->GetLogDevs(); i++) {
			CSoundDevice* pdev = theConfig->GetLogDeviceFromIndex(i);
			if (pdev) {
				pdev->TimerCallBack(tick);
			}
		}
		for (int i = 0; i < theConfig->GetPcmDevs(); i++) {
			CAdPcmBase* pdev = theConfig->GetPCMDeviceFromIndex(i);
			if (pdev) {
				pdev->TimerCallBack(tick);
			}
		}
		theConfig->TimerCallBack();
		timerprocessing = 0;
	}
	else {
		fprintf(stderr, _T("%i: Interrupt passed\n"), tick);
	}
}

void CFITOM::MIDIClockCallBack()
{
	MIDItick++;
	for (int i = 0; i < theConfig->GetMidiInputs(); i++) {
		midi[i]->MIDIClockCallBack(MIDItick);
	}
}

int CFITOM::ImportConfig(CFITOMConfig* cfg)
{
	int res = 0;
	theConfig = cfg;
	int clockch = cfg->GetMIDIClockCh();
	for (int i = 0; i < theConfig->GetMidiInputs(); i++) {
		CMidiIn* pMidiIn = cfg->GetMidiIn(i);
		midi[i] = new CMidiInst(pMidiIn, this, (i == clockch));
		if (midi[i]) {
			//pMidiIn->RegistInterrupt(midi[i]);
			for (int j = 0; j < 16; j++) {
				int devid = cfg->GetChannelMapDevice(j);
				if (devid > 0) {
					if (devid == DEVICE_RHYTHM) {
						midi[i]->AssignRhythm(j);
					}
					else {
						CSoundDevice* psd = cfg->GetLogDeviceFromID(devid);
						psd = psd ? psd : cfg->GetLogDeviceFromIndex(0);
						midi[i]->AssignInst(j, psd, cfg->GetChannelMapPoly(j));
					}
				}
				else {
					midi[i]->AssignInst(j, NULL, 0);
				}
			}
		}
	}
	return res;
}

void CFITOM::ReloadVoice(FMVOICE* voice, UINT32 dev, UINT8 bank, UINT8 num)
{
	for (int i = 0; i < theConfig->GetMidiInputs(); i++) {
		for (int j = 0; j < 16; j++) {
			CMidiCh* pch = midi[i]->GetMidiCh(j);
			if (pch->IsInst() && pch->GetDeviceID() == dev && pch->GetBankNo() == bank && pch->GetProgramNo() == num) {
				pch->ProgChange(num);
			}
		}
	}
}

CFITOM::cmdvect CFITOM::commandtable[] = {
{ "SetPresetVoice", &CFITOM::cmdSetVoice, },
{ "GetPresetVoice", &CFITOM::cmdGetVoice, },
{ "GetMidiStatus", &CFITOM::cmdGetMidiStatus, },
{ "GetDeviceStatus", &CFITOM::cmdGetDeviceStatus, },
{ "SendMidi", &CFITOM::cmdSendMidi, },
{ "GetPhysicalDevice", &CFITOM::cmdGetPhysicalDevice, },
{ "GetLogicalDevice", &CFITOM::cmdGetLogicalDevice, },
{ "GetMidiDevice", &CFITOM::cmdGetMidiDevice, },
{ "SetPcmVoice", &CFITOM::cmdSetPcmVoice, },
{ "GetPcmVoice", &CFITOM::cmdGetPcmVoice, },
{ "SetWaveForm", &CFITOM::cmdSetWaveForm, },
{ "GetWaveForm", &CFITOM::cmdGetWaveForm, },
{ "SetDeviceReg", &CFITOM::cmdSetDeviceReg, },
{ "GetDeviceReg", &CFITOM::cmdGetDeviceReg, },
};

BOOL CFITOM::CmdProc(TCHAR* cmd, TCHAR* result, int reslen)
{
	BOOL ret = false;
	TCHAR cmdstr[128];
	TCHAR param[65536];
	sscanf_s(cmd, _T("^[,\s],%s"), cmdstr, _countof(cmdstr), param, _countof(param));
	size_t len = strnlen(cmdstr, _countof(cmdstr));
	for each (cmdvect var in commandtable)
	{
		if (tcscmpn(var.cmd, cmdstr, len) == 0) {
			return (this->*(var.func))(param, result, reslen);
		}
	}
	return ret;
}