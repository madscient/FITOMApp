#include "stdafx.h"
#include "FITOM.h"
#include "OPN.h"
#include "OPM.h"
#include "OPL.h"
#include "OPLL.h"
#include "SSG.h"
#include "MIDIDEV.h"
#include "MIDI.h"
#include "SCCIWrapper.h"
#include "FITOMCfg.h"
#include "tables.h"

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

CFITOM::CFITOM() : MIDItick(0), MasterVolume(127), timerprocessing(0), pollprocessing(0), timerskipped(0), theConfig(0)
{
	//Multiple Oscilator Technology Integration Firmware
	fprintf(stderr, _T("FM Instruments Total Operating Middleware\n\n"));
}

CFITOM::~CFITOM()
{
	fprintf(stderr, _T("Closing FITOM...\n\n"));
}

void CFITOM::ExitInstance(int save)
{
}


CFMBank* CFITOM::GetFMBank(UINT8 voiceid, UINT8 bank)
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

const UINT8 CFITOM::GetDeviceVoiceType(UINT8 devid)
{
	for (int i = 0; i<(sizeof(ROM::devmap) / sizeof(ROM::DEVIDMAP)); i++) {
		if (ROM::devmap[i].devid == devid) {
			return ROM::devmap[i].voicetype;
		}
	}
	return 0;
}

const UINT8 CFITOM::GetDeviceVoiceGroupMask(UINT8 devid)
{
	for (int i = 0; i<(sizeof(ROM::devmap) / sizeof(ROM::DEVIDMAP)); i++) {
		if (ROM::devmap[i].devid == devid) {
			return ROM::devmap[i].voicegroup;
		}
	}
	return 0;
}

const DWORD CFITOM::GetDeviceRegSize(UINT8 devid)
{
	for (int i = 0; i<(sizeof(ROM::devmap) / sizeof(ROM::DEVIDMAP)); i++) {
		if (ROM::devmap[i].devid == devid) {
			return ROM::devmap[i].regsize;
		}
	}
	return 0;
}

UINT8* CFITOM::GetCompatiList(UINT8 devid)
{
	UINT8* retlist = 0;
	if (devid < (sizeof(ROM::compatilist) / sizeof(UINT8))) {
		retlist = ROM::compatilist[devid];
	}
	return retlist;
}

const UINT8 CFITOM::GetDeviceIDFromName(LPCTSTR name)
{
	for (int i = 0; i<(sizeof(ROM::devmap) / sizeof(ROM::DEVIDMAP)); i++) {
		if (strcmp(ROM::devmap[i].chipname, name) == 0 || strcmp(ROM::devmap[i].chipcode, name) == 0) {
			return ROM::devmap[i].devid;
		}
	}
	return DEVICE_NONE;
}

const TCHAR* CFITOM::GetDeviceNameFromID(UINT8 devid)
{
	for (int i = 0; i<(sizeof(ROM::devmap) / sizeof(ROM::DEVIDMAP)); i++) {
		if (ROM::devmap[i].devid == devid) {
			return ROM::devmap[i].chipname;
		}
	}
	return _T("-----");
}

int CFITOM::GetClockCode(UINT32 clock)
{
	int clockcode = 0;
	switch (clock) {
	case 1789772:
	case 3579545:
	case 7159090:
	case 14318180:
		clockcode = CLKCODE358;
		break;
	case 1996800:
	case 3993600:
	case 7987200:
	case 15974400:
		clockcode = CLKCODE399;
		break;
	case 2000000:
	case 4000000:
	case 8000000:
	case 16000000:
		clockcode = CLKCODE400;
		break;
	case 2048000:
	case 4096000:
	case 8192000:
	case 16384000:
		clockcode = CLKCODE409;
		break;
	case 1917610:
	case 3835221:
	case 7670442:
	case 15340885:
		clockcode = CLKCODE383;
		break;
	default:
		break;
	}
	return clockcode;
}

int CFITOM::GetVoice(FMVOICE* voice, UINT8 dev, UINT8 bank, UINT8 prog)
{
	CFMBank* xmem = NULL;
	UINT8 vtype = GetDeviceVoiceGroupMask(dev);
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
			std::sprintf(voice->name, "PCM:%03i:%03i", bank, prog);
		}
		return 0;
	}
	else if (vtype == VOICE_GROUP_OPL4) {
		memset(voice, 0, sizeof(FMVOICE));
		voice->ID = (0xff000000L) | (UINT16(bank) << 8) | prog;
		voice->AL = prog;
		voice->FB = bank;
		{
			std::sprintf(voice->name, "OPL4:%03i:%03i", bank, prog);
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
	UINT8 vtype = GetDeviceVoiceGroupMask(dev);
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
	if (drum) {
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
			pMidiIn->RegistInterrupt(midi[i]);
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
