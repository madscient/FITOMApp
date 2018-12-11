#include "stdafx.h"
#include "FITOMdefine.h"
#include "FITOM.h"
#include "OPN.h"
#include "OPM.h"
#include "OPL.h"
#include "OPLL.h"
#include "SSG.h"
#include "ADPCM.h"
#include "MIDIDEV.h"
#include "MIDI.h"
#include "SCCIWrapper.h"
#include "FITOMCfg.h"
#include "tables.h"
#include "../pthread/semaphore.h"
#include "../pthread/pthread.h"

const char* FITOMTITLE = "FITOM Windows";

int CFITOM::InitInstance(CFITOMConfig* config)
{
#ifdef _DEBUG
	g_devno = 0;
#endif
	int res = ImportConfig(config);
	if (res) { return res; }
	for (int i=0; i<theConfig->GetLogDevs(); i++) {
		CSoundDevice* pdev = theConfig->GetLogDeviceFromIndex(i);
		pdev->Reset();
		FMVOICE voice;
		GetVoice(&voice, pdev->GetDevice(), 0, 0);
		for (int j=0; j<pdev->GetAvailable(); j++) {
			//fmdev[i]->ResetVoice(NULL, NULL, 0);
			pdev->SetVoice(j, &voice);
		}
	}
	ResetAllCtrl();
	AllNoteOff();
	return 0;
}

CFITOM::CFITOM() : MIDItick(0), timerprocessing(0), pollprocessing(0), theConfig(0), LCDdisp(0)
{
	//Multiple Oscilator Technology Integration Firmware
	std::cerr << _T("FM Instruments Total Operating Middleware") << std::endl << std::endl;
}

CFITOM::~CFITOM()
{
	std::cerr << _T("Closing FITOM...") << std::endl << std::endl;
}

void CFITOM::ExitInstance(int save)
{
}


CFMBank* CFITOM::GetFMBank(UINT32 voiceid, UINT32 bank)
{
	return theConfig ? theConfig->GetFMBank(voiceid, bank) : 0;
}

CDrumBank* CFITOM::GetDrumBank(UINT8 prog)
{
	return theConfig ? theConfig->GetDrumBank(prog) : 0;
}

CPcmBank* CFITOM::GetPcmBank(UINT8 bank)
{
	return theConfig ? theConfig->GetPcmBank(bank) : 0;
}

const DWORD CFITOM::GetDeviceVoiceType(DWORD devid)
{
	for (int i = 0; ROM::devmap[i].devid != DEVICE_NONE; i++) {
		if (ROM::devmap[i].devid == devid) {
			return ROM::devmap[i].voicetype;
		}
	}
	return 0;
}

const DWORD CFITOM::GetDeviceVoiceGroupMask(DWORD devid)
{
	for (int i = 0; ROM::devmap[i].devid != DEVICE_NONE; i++) {
		if (ROM::devmap[i].devid == (devid & 0xff)) {
			return ROM::devmap[i].voicegroup;
		}
	}
	return 0;
}

const DWORD CFITOM::GetDeviceRegSize(DWORD devid)
{
	for (int i = 0; ROM::devmap[i].devid != DEVICE_NONE; i++) {
		if (ROM::devmap[i].devid == devid) {
			return ROM::devmap[i].regsize;
		}
	}
	return 0;
}

DWORD* CFITOM::GetCompatiList(DWORD devid)
{
	DWORD* retlist = 0;
	if (devid < (sizeof(ROM::compatilist) / sizeof(DWORD))) {
		retlist = ROM::compatilist[devid];
	}
	return retlist;
}

const DWORD CFITOM::GetDeviceIDFromName(LPCTSTR name)
{
	for (int i = 0; ROM::devmap[i].devid != DEVICE_NONE; i++) {
		if (strcmp(ROM::devmap[i].chipname, name) == 0 || strcmp(ROM::devmap[i].chipcode, name) == 0) {
			return ROM::devmap[i].devid;
		}
	}
	return DEVICE_NONE;
}

const TCHAR* CFITOM::GetDeviceNameFromID(DWORD devid)
{
	if (devid & 0x80000000) {
		devid &= 0x7fffffff;
	}
	for (int i = 0; ROM::devmap[i].devid != DEVICE_NONE; i++) {
		if (ROM::devmap[i].devid == devid) {
			return ROM::devmap[i].chipname;
		}
	}
	return _T("-----");
}

int CFITOM::GetVoice(FMVOICE* voice, UINT8 dev, UINT8 bank, UINT8 prog)
{
	CFMBank* xmem = NULL;
	UINT32 vtype = GetDeviceVoiceGroupMask(dev);
	if (vtype == VOICE_GROUP_PCM) {
		memset(voice, 0, sizeof(FMVOICE));
		voice->ID = (0xff000000L) | (UINT16(bank) << 8) | prog;
		voice->AL = prog;
		CPcmBank* pbank = theConfig->GetPcmBank(bank);
		if (pbank) {
			PCMPROG pcmprog;
			pbank->GetVoice(prog, &pcmprog);
			strncpy(voice->name, pcmprog.progname, 15);
		}
		else {
			std::sprintf(voice->name, _T("PCM:%03i:%03i"), bank, prog);
		}
		return 0;
	}
	else if (vtype == VOICE_GROUP_OPL4 || vtype == VOICE_GROUP_RHYTHM) {
		memset(voice, 0, sizeof(FMVOICE));
		voice->ID = (0xff000000L) | (UINT16(bank) << 8) | prog;
		voice->AL = prog;
		voice->FB = bank;
		{
			std::sprintf(voice->name, _T("BuiltIn:%03i:%03i"), bank, prog);
		}
		return 0;
	}
	if (bank < MAX_BANK && prog < 128 && (xmem = GetFMBank(vtype, bank))) {
		xmem->GetVoice(prog, voice);
	}
	if (xmem == NULL) {
		memset(voice, 0, sizeof(FMVOICE));
		voice->ID = (0xff000000L) | (UINT16(bank) << 8) | prog;
		std::sprintf(voice->name, _T("%03i:%03i"), bank, prog);
	}
	return 0;
}

int CFITOM::SetVoice(FMVOICE* voice, UINT8 dev, UINT8 bank, UINT8 prog)
{
	CFMBank* xmem = NULL;
	UINT32 vtype = GetDeviceVoiceGroupMask(dev);
	if (bank < MAX_BANK && prog < 128 && (xmem = GetFMBank(vtype, bank))) {
		xmem->SetVoice(prog, voice);
	} else {
		return -1;
	}
	return 0;
}

int CFITOM::GetDrum(DRUMMAP* drum, UINT8 bank, UINT8 prog, UINT8 note)
{
	int ret = -1;
	if (note < 128 && drum) {
		prog &= 0x7f;
		note &= 0x7f;
		CDrumBank* drummap = theConfig->GetDrumBank(prog);
		if (drummap) {
			drummap->GetVoice(note, drum);
			ret = 0;
		}
	}
	return ret;
}

void CFITOM::AllNoteOff()
{
	for (int i=0; i<theConfig->GetLogDevs(); i++) {
		CSoundDevice* pdev = theConfig->GetLogDeviceFromIndex(i);
		if (pdev) {
			pdev->Reset();
		}
	}
}

void CFITOM::ResetAllCtrl()
{
	for (int i = 0; i<theConfig->GetMidiInputs(); i++) {
		for (int j=0; j<16; j++) {
			CMidiCh* mch = midi[i]->GetMidiCh(j);
			if (mch) {
				mch->ResetAllCtrl();
			}
		}
	}
	std::strncpy((char*)LCDstr, FITOMTITLE, 16);
	std::memset(LCDdot, 0, 16 * 16 * 11);
	theConfig->SetMasterVolume(127);
	LCDdisp = 0;
}

int CFITOM::PollingCallBack()
{
	int i;
	int ret = 0;
	if (!timerprocessing && !pollprocessing) {
		pollprocessing = 1;
		for (i = 0; i<theConfig->GetMidiInputs(); i++) {
			ret += midi[i]->PollingCallBack();
		}
		for (int i = 0; i<theConfig->GetLogDevs(); i++) {
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
		for (i = 0; i<theConfig->GetMidiInputs(); i++) {
			midi[i]->TimerCallBack(tick);
		}
		for (int i = 0; i<theConfig->GetLogDevs(); i++) {
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

int CFITOM::CommandCallBack(TCHAR* cmd, TCHAR* param)
{
	int ret = 0;
	return ret;
}