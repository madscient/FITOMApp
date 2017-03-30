#include "stdafx.h"
#include "FITOM.h"
#include "SoundDev.h"
#include "ADPCM.h"
#include "FITOMCfg.h"
#include "MIDIDEV.h"
#include "SCCIWrapper.h"
#include "Port.h"
#include "MIDI.h"
#include "OPM.h"
#include "OPN.h"
#include "OPL.h"
#include "OPLL.h"
#include "OPK.h"
#include "SSG.h"

CFITOMConfig::CFITOMConfig() : phydevs(0), logdevs(0), pcmdevs(0), mpus(0)
{
	for (int i = 0; i < MAX_MPUS; i++) {
		vMidiIn[i] = 0;
	}
	for (int i = 0; i < MAX_DEVS; i++) {
		vPhyDev[i] = 0;
		vLogDev[i] = 0;
		vPcmDev[i] = 0;
	}
	for (int i = 0; i < MAX_BANK; i++) {
		vOpmBank[i] = 0;
		vOpnBank[i] = 0;
		vOpl2Bank[i] = 0;
		vOpl3Bank[i] = 0;
		vOpllBank[i] = 0;
		vPsgBank[i] = 0;
		vDrumBank[i] = 0;
	}
	for (int i = 0; i < 16; i++) {
		channelMap[i].device_id = channelMap[i].polyphon = 0;
	}
}

CFITOMConfig::~CFITOMConfig()
{
	/*
	delete[] vMidiIn;
	delete[] vPhyDev;
	delete[] vLogDev;
	delete[] vOpmBank;
	delete[] vOpnBank;
	delete[] vOpl2Bank;
	delete[] vOpl3Bank;
	delete[] vOpl3Bank;
	delete[] vPsgBank;
	delete[] vDrumBank;
	*/
}

CFMBank* CFITOMConfig::AllocFMBank(int voicegroup, int bank)
{
	CFMBank* ret = 0;
	if (bank < MAX_BANK) {
		switch (voicegroup) {
		case VOICE_GROUP_OPM:
			ret = vOpmBank[bank] = new CFMBank();
			break;
		case VOICE_GROUP_OPNA:
			ret = vOpnBank[bank] = new CFMBank();
			break;
		case VOICE_GROUP_OPL2:
			ret = vOpl2Bank[bank] = new CFMBank();
			break;
		case VOICE_GROUP_OPL3:
			ret = vOpl3Bank[bank] = new CFMBank();
			break;
		case VOICE_GROUP_OPLL:
			ret = vOpllBank[bank] = new CFMBank();
			break;
		case VOICE_GROUP_PSG:
			ret = vPsgBank[bank] = new CFMBank();
			break;
		}
	}
	return ret;
}

CFMBank* CFITOMConfig::GetFMBank(int voicegroup, int bank)
{
	CFMBank* ret = 0;
	if (bank < MAX_BANK) {
		switch (voicegroup) {
		case VOICE_GROUP_OPM:
			ret = vOpmBank[bank];
			break;
		case VOICE_GROUP_OPNA:
			ret = vOpnBank[bank];
			break;
		case VOICE_GROUP_OPL2:
			ret = vOpl2Bank[bank];
			break;
		case VOICE_GROUP_OPL3:
			ret = vOpl3Bank[bank];
			break;
		case VOICE_GROUP_OPLL:
			ret = vOpllBank[bank];
			break;
		case VOICE_GROUP_PSG:
			ret = vPsgBank[bank];
			break;
		}
	}
	return ret;
}

CDrumBank* CFITOMConfig::AllocDrumBank(int prog)
{
	CDrumBank* ret = 0;
	if (prog < MAX_BANK) {
		ret = vDrumBank[prog] = new CDrumBank();
	}
	return ret;
}

CDrumBank* CFITOMConfig::GetDrumBank(int prog)
{
	CDrumBank* ret = 0;
	if (prog < MAX_BANK) {
		ret = vDrumBank[prog];
	}
	return ret;
}

CPcmBank* CFITOMConfig::AllocPcmBank(int prog)
{
	CPcmBank* ret = 0;
	if (prog < MAX_BANK) {
		ret = vPcmBank[prog] = new CPcmBank();
	}
	return ret;
}

CPcmBank* CFITOMConfig::GetPcmBank(int prog)
{
	CPcmBank* ret = 0;
	if (prog < MAX_BANK) {
		ret = vPcmBank[prog];
	}
	return ret;
}

CAdPcmBase* CFITOMConfig::AddDevice(CAdPcmBase* pdev)
{
	//vPhyDev[phydevs++] = pdev;
	vPcmDev[pcmdevs++] = pdev;
	return pdev;
}

CSoundDevice* CFITOMConfig::AddDevice(CSoundDevice* pdev)
{
	TCHAR tmp1[80], tmp2[80], tmp3[80], tmp4[80];
	if (logdevs >= MAX_DEVS || !pdev) return 0;
	pdev->GetDescriptor(tmp1, 80);
	if (pdev->GetDevPort()) {
		pdev->GetDevPort()->GetDesc(tmp2, 80);
	}
	vPhyDev[phydevs++] = pdev;
	for (int i = 0; i<logdevs; i++) {
		if (pdev->GetDevice() == vLogDev[i]->GetDevice()	//デバイスタイプが一致
			//&& pdev->GetDevPort()->GetClock() == vLogDev[i]->GetDevPort()->GetClock()	//クロックが一致
			) {
			// 同一デバイスは束ねる
			vLogDev[i]->GetDescriptor(tmp3, 80);
			CPort* pt = vLogDev[i]->GetDevPort();
			if (pt) {
				pt->GetDesc(tmp4, 80);
				fprintf(stderr, _T("Dev.%i: %s+%s (spanned) port=%s%s\n"), i, tmp1, tmp3, tmp2, tmp4);
				CSpanDevice* pspan = new CSpanDevice(pdev, vLogDev[i]);
				vLogDev[i] = pspan;
			}
			else { //already spanned
				((CSpanDevice*)vLogDev[i])->AddDevice(pdev);
			}
			return vLogDev[i];
		}
	}
	fprintf(stderr, _T("Dev.%i: %s port=%s\n"), logdevs, tmp1, tmp2);
	vLogDev[logdevs++] = pdev;
	return pdev;
}

int CFITOMConfig::AddDevice(CMidiIn* pmpu)
{
	int ret = -1;
	if (mpus < MAX_MPUS && pmpu) {
		ret = mpus++;
		vMidiIn[ret] = pmpu;
		//vMidiInst[ret] = new CMidiInst(pmpu, NULL);
	}
	return ret;
}

int CFITOMConfig::CreateADPCMDevice(int devtype, TCHAR* param)
{
	int res = -1;
	TCHAR devname[80];
	TCHAR portparams[80];
	sscanf_s(param, _T("%[^,], %[^\0]"), devname, _countof(devname), portparams, _countof(portparams));
	int pardev = CFITOM::GetDeviceIDFromName(devname);
	CPort* pt;
	CPort* pt2 = 0;
	if (pt = CreatePort(pardev, (TCHAR*)portparams)) {
		int fs = pt->GetClock();
		CAdPcmBase* pdev = 0;
		switch (pardev) {
		case DEVICE_Y8950:
			pdev = AddDevice(new CAdPcm3801(pt, fs, 256 * 1024));
			break;
		case DEVICE_OPNA:
			pt2 = new COffsetPort(pt, 0x100);
			pdev = AddDevice(new CAdPcm2608(pt2, fs, 256 * 1024));
			break;
		case DEVICE_PCMD8:
			pdev = AddDevice(new CAdPcmZ280(pt, fs, 4096 * 1024));
			break;
		case DEVICE_OPNB:
		case DEVICE_2610B:
		case DEVICE_F286:
			break;
		}
		if (pdev) { res = 0; }
	}
	return res;
}

int CFITOMConfig::CreateSingleDevice(int devtype, TCHAR* param)
{
	int res = 0;
	int md;
	TCHAR portparams[80];
	sscanf_s(param, _T("%i, %s"), &md, portparams, _countof(portparams));
	CPort* pt = 0;
	CPort* pt2 = 0;
	if ((pt = CreatePort(devtype, (TCHAR*)portparams))) {
		int fs = pt->GetClock();
		switch (devtype) {
		case DEVICE_OPN:
			AddDevice(new COPN(pt, fs));
			AddDevice(new CSSG(pt, fs/2));
			break;
		case DEVICE_OPNC:
			AddDevice(new COPNC(pt, fs));
			AddDevice(new CSSG(pt, fs/2));
			break;
		case DEVICE_OPM:
			AddDevice(new COPM(pt, fs));
			break;
		case DEVICE_OPP:
			AddDevice(new COPP(pt, fs));
			break;
		case DEVICE_OPZ:
			AddDevice(new COPZ(pt, fs));
			break;
		case DEVICE_OPN2:
			pt2 = new COffsetPort(pt, 0x100);
			AddDevice(new COPN2(pt, pt2, fs));
			break;
		case DEVICE_OPN2C:
			pt2 = new COffsetPort(pt, 0x100);
			AddDevice(new COPN2C(pt, pt2, fs));
			break;
		case DEVICE_OPN2L:
			pt2 = new COffsetPort(pt, 0x100);
			AddDevice(new COPN2L(pt, pt2, fs));
			break;
		case DEVICE_OPNB:
			pt2 = new COffsetPort(pt, 0x100);
			AddDevice(new COPNB(pt, pt2, fs));
			AddDevice(new CSSG(pt, fs / 2));
			break;
		case DEVICE_F286:
			pt2 = new COffsetPort(pt, 0x100);
			AddDevice(new CF286(pt, pt2, fs));
			AddDevice(new CSSG(pt, fs / 2));
			break;
		case DEVICE_2610B:
			pt2 = new COffsetPort(pt, 0x100);
			AddDevice(new C2610B(pt, pt2, fs));
			AddDevice(new CSSG(pt, fs / 2));
			break;
		case DEVICE_OPNA:
			pt2 = new COffsetPort(pt, 0x100);
			AddDevice(new COPNA(pt, pt2, fs));
			AddDevice(new CSSG(pt, fs));
			break;
		case DEVICE_OPN3L:
			pt2 = new COffsetPort(pt, 0x100);
			AddDevice(new COPN3L(pt, pt2, fs));
			AddDevice(new CSSG(pt, fs/2));
			break;
		case DEVICE_OPL3:
			pt2 = new COffsetPort(pt, 0x100);
			switch (md) {
			case 0:	// 6op4 + 6op2
				AddDevice(new COPL3(pt, fs));
				AddDevice(new COPL3_2(pt, pt2, 0, fs));
				break;
			case 1:	// 18op2
				AddDevice(new COPL3_2(pt, pt2, UINT8(md), fs));
				break;
			}
			break;
		case DEVICE_OPL:
			AddDevice(new COPL(pt, fs));
			break;
		case DEVICE_Y8950:
			AddDevice(new C3801(pt, fs));
			break;
		case DEVICE_OPL2:
			AddDevice(new COPL2(pt, fs));
			break;
		case DEVICE_OPLL:
			AddDevice(new COPLL(pt, UINT8(md), fs));
			break;
		case DEVICE_OPLL2:
			AddDevice(new COPLL2(pt, UINT8(md), fs));
			break;
		case DEVICE_OPLLP:
			AddDevice(new COPLLP(pt, UINT8(md), fs));
			break;
		case DEVICE_OPLLX:
			AddDevice(new COPLLX(pt, UINT8(md), fs));
			break;
		case DEVICE_OPK:
			AddDevice(new COPK(pt, fs));
			break;
		case DEVICE_OPK2:
			AddDevice(new COPK2(pt, fs));
			break;
		case DEVICE_DSG:
			AddDevice(new CDSG(new COffsetPort(pt, 0x80), fs));
			break;
		case DEVICE_SSG:
			AddDevice(new CSSG(pt, fs));
			break;
		case DEVICE_APSG:
			AddDevice(new CAPSG(pt, fs));
			break;
		case DEVICE_DCSG:
			AddDevice(new CDCSG(pt, fs));
			break;
		default:
			res = -1;
			delete pt;
		}
	}
	return res;
}

CSoundDevice* CFITOMConfig::GetPhysDeviceFromID(UINT32 id) const
{
	for (int i = 0; i < phydevs; i++) {
		CPort* pt = (vPhyDev[i]) ? vPhyDev[i]->GetDevPort() : 0;
		if (pt && pt->GetPhysicalId() == id) {
			return vPhyDev[i];
		}
	}
	return 0;
}

const int CFITOMConfig::GetLogDeviceIDFromIndex(UINT8 i) const
{
	return (i < logdevs) ? vLogDev[i]->GetDevice() : DEVICE_NONE;
}

int CFITOMConfig::GetLogDeviceIndex(CSoundDevice* pdev)
{
	for (int i = 0; i<logdevs; i++) {
		if (pdev == vLogDev[i]) {
			return i;
		}
	}
	return -1;
}

int CFITOMConfig::GetLogDeviceIndex(UINT8 devid)
{
	for (int i = 0; i<logdevs; i++) {
		if (vLogDev[i]->GetDevice() == devid) {
			return i;
		}
	}
	return -1;
}

CSoundDevice* CFITOMConfig::GetLogDeviceFromID(UINT8 devid) const
{
	CSoundDevice* ret = 0;
	if (devid != DEVICE_RHYTHM) {
		for (int i = 0; i < logdevs; i++) {
			if (vLogDev[i]->GetDevice() == devid) {
				ret = vLogDev[i];
				break;
			}
		}
		if (ret == 0) {
			UINT8* cmplst = CFITOM::GetCompatiList(devid);
			if (cmplst) {
				for (int k = 0; (cmplst[k] != DEVICE_NONE) && (ret == 0); k++) {
					for (int i = 0; i < logdevs; i++) {
						if (vLogDev[i]->GetDevice() == cmplst[k]) {
							ret = vLogDev[i];
							break;
						}
					}
				}
			}
		}
	}
	return ret;
}

CAdPcmBase* CFITOMConfig::GetPCMDeviceFromID(UINT32 devid)
{
	CAdPcmBase* ret = 0;
	for (int i = 0; i<pcmdevs; i++) {
		if (devid == GetDeviceUniqID(vPcmDev[i])) {
			ret = vPcmDev[i];
			break;
		}
	}
	return ret;
}

int CFITOMConfig::GetPcmDeviceIndex(CAdPcmBase* pdev)
{
	for (int i = 0; i<pcmdevs; i++) {
		if (pdev == vPcmDev[i]) {
			return i;
		}
	}
	return -1;
}

int CFITOMConfig::GetPcmDeviceIndex(UINT32 devid)
{
	for (int i = 0; i<pcmdevs; i++) {
		if (vPcmDev[i]->GetDevice() == devid) {
			return i;
		}
	}
	return -1;
}

CSoundDevice* CFITOMConfig::GetDeviceFromUniqID(UINT32 uid)
{
	CSoundDevice* ret = 0;
	UINT vg = CFITOM::GetDeviceVoiceGroupMask(uid & 0xff);
	if (uid & 0x80000000) {
		ret = GetLogDeviceFromID(uid & 0xff);
	}
	else if (vg == VOICE_GROUP_PCM) {
		ret = GetPCMDeviceFromID(uid);
	}
	else {
		ret = GetPhysDeviceFromID(uid);
	}
	return ret;
}

UINT32 CFITOMConfig::GetDeviceUniqID(CSoundDevice* pdev, int phys)
{
	UINT32 ret = 0;
	if (pdev) {
		ret = phys ? (pdev->GetDevPort()->GetPhysicalId() & 0xffffff00) | pdev->GetDevice() : 0x80000000 | pdev->GetDevice();
	}
	return ret;
}

UINT32 CFITOMConfig::GetDeviceUniqID(CAdPcmBase* pdev)
{
	return pdev ? (pdev->GetDevPort()->GetPhysicalId() & 0xffff00) | (pdev->GetParentDev() << 24) | pdev->GetDevice() : 0;
}

const TCHAR* CFITOMConfig::GetBankName(UINT32 devid, UINT32 bank)
{
	const TCHAR* ret = 0;
	if (isPcmDevice(devid)) {
		if (bank < pcmdevs) {
			ret = vPcmBank[bank]->GetBankName();
		}
	}
	else if (devid == RHYTHM_BANK) {
		ret = _T("RHYTHM");	//Nothing
	}
	else {
		CFMBank* pbank = GetFMBank(CFITOM::GetDeviceVoiceGroupMask(devid & 0xff), bank);
		if (pbank) {
			ret = pbank->GetBankName();
		}
	}
	if (ret == 0) {
		ret = _T("--------");
	}
	return ret;
}

int CFITOMConfig::GetVoiceName(UINT32 devid, UINT32 bank, UINT32 prog, TCHAR* name, size_t count)
{
	int ret = 0;
	*name = _T('\0');
	if (isPcmDevice(devid)) {
		if (bank < pcmdevs) {
			PCMPROG pcmprog;
			vPcmBank[bank]->GetVoice(prog, &pcmprog);
			StringCchCopy(name, count, pcmprog.progname);
			ret = lstrlen(name);
		}
	}
	else if (devid == RHYTHM_BANK) {
		if (prog < MAX_BANK && vDrumBank[prog]) {
			StringCchCopy(name, count, vDrumBank[prog]->GetBankName());
			ret = lstrlen(name);
		}
	}
	else {
		CFMBank* pbank = GetFMBank(CFITOM::GetDeviceVoiceGroupMask(devid & 0xff), bank);
		if (pbank) {
			FMVOICE voice;
			pbank->GetVoice(prog, &voice);
			char vname[17];
			memcpy(vname, voice.name, 16);
			vname[16] = '\0';
			StringCchCopy(name, count, CA2T(vname));
			ret = lstrlen(name);
		}
	}
	if (ret == 0) {
		lstrcpyn(name, _T("--------"), count);
		ret = lstrlen(name);
	}
	return ret;
}

int CFITOMConfig::GetDeviceName(UINT32 devid, TCHAR* name, size_t count)
{
	int ret = 0;
	const TCHAR* devname = CFITOM::GetDeviceNameFromID(devid & 0xff);
	if (isPcmDevice(devid)) {
		ret = StringCchPrintf(name, count, _T("%s(%s)"), devname, CFITOM::GetDeviceNameFromID(devid >> 24));
	}
	else {
		ret = StringCchPrintf(name, count, _T("%s"), devname);
	}
	return ret;
}

// CFITOMConfigWin32

CFITOMConfigWin32::CFITOMConfigWin32() : CFITOMConfig(), g_devno(0), pScci(new CSCCIWrapper())
{
}

CFITOMConfigWin32::~CFITOMConfigWin32()
{
	delete pScci;
}

CPort* CFITOMConfigWin32::CreatePort(int devtype, TCHAR* params)
{
	CPort* ret = 0;
	TCHAR iftype[16];
	TCHAR portdef[16];
	DWORD ifid;
	DWORD slid;
	sscanf_s(params, _T("%[^:]: %s"), iftype, _countof(iftype), portdef, _countof(portdef));
	if (lstrcmp(iftype, _T("SCCI")) == 0) {
		SC_CHIP_TYPE chiptype = pScci->getScChipType(devtype);
		if (lstrcmp(portdef, _T("AUTO")) == 0) {
			SoundChip* chip = pScci->getSoundChipFromType(chiptype);
			if (chip) {
				SCCI_SOUND_CHIP_INFO* sci = chip->getSoundChipInfo();
				ifid = pScci->getInterfaceIDFromChip(chip);
				SoundInterface* psi = pScci->getSoundInterface(ifid);
				ret = new CSCCIPort(psi, chip, size_t(CFITOM::GetDeviceRegSize(devtype)));
				slid = pScci->getSlotIDFromChip(ifid, chip);
			}
		}
		else {
			sscanf_s(portdef, _T("%i : %i"), &ifid, &slid);
			SoundChip* chip = pScci->getSoundChipFromId(ifid, slid);
			SoundInterface* psi = pScci->getSoundInterface(ifid);
			if (chip && psi) {
				ret = new CSCCIPort(psi, chip, size_t(CFITOM::GetDeviceRegSize(devtype)));
			}
		}
	}
	if (!ret) {
		fprintf(stderr, _T("Failed to init port: %s\n"), params);
	}
	if (ret) {
		ret->SetPhysicalId((ifid << 16) | (slid << 8) | devtype);
	}
	return ret;
}

CMidiIn* CFITOMConfigWin32::CreateMidiInPort(TCHAR* param)
{
	CMidiIn* ret = new CW32MidiIn(param);
	return ret;
}

int CFITOMConfigWin32::CreateDevice(TCHAR* param)
{
	TCHAR devname[80];
	TCHAR portparam[MAX_PATH];
	sscanf_s(param, _T("%[^:]:%[^\0]"), devname, _countof(devname), portparam, _countof(portparam));
	int devtype = CFITOM::GetDeviceIDFromName(devname);
	if (devtype == DEVICE_ADPCM || devtype == DEVICE_ADPCMA || devtype == DEVICE_ADPCMB) {
		return CreateADPCMDevice(devtype, portparam);
	}
	return CreateSingleDevice(devtype, portparam);
}

