#include "stdafx.h"

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/foreach.hpp>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>

#include "FITOM.h"
#include "SoundDev.h"
#include "ADPCM.h"
#include "FITOMCfg.h"
#include "MIDIDEV.h"
#include "Port.h"
#include "MIDI.h"
#include "OPM.h"
#include "OPN.h"
#include "OPL.h"
#include "OPLL.h"
#include "OPK.h"
#include "SSG.h"
#include "codec.h"

CFITOMConfig::CFITOMConfig(LPCTSTR strinifile) : phydevs(0), logdevs(0), pcmdevs(0), mpus(0), pProgressMessage(0), pProgressFilename(0)
{
	boost::property_tree::read_ini(_T(".\\FITOM.ini"), fitom_ini);
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

int CFITOMConfig::CreateDevice(LPCTSTR param)
{
	std::tstring strparam(param);
	std::tstring strdevname;
	std::tstring strport;
	strdevname = strparam.substr(0, strparam.find_first_of(_T(':')));
	strport = strparam.substr(strparam.find_first_of(_T(':')) + 1);
	int devtype = CFITOM::GetDeviceIDFromName(strdevname.c_str());
	if (devtype == DEVICE_ADPCM || devtype == DEVICE_ADPCMA || devtype == DEVICE_ADPCMB) {
		return CreateADPCMDevice(devtype, strport.c_str());
	}
	return CreateSingleDevice(devtype, strport.c_str());
}

int CFITOMConfig::CreateADPCMDevice(int devtype, LPCTSTR param)
{
	int res = -1;
	std::tstring strparam(param);
	std::tstring strdevname;
	std::tstring strport;
	std::vector<std::tstring> lstdev;
	boost::split(lstdev, strparam, boost::is_any_of(_T(",")));
	if (lstdev.size() < 2) { return -1; }
	strdevname = lstdev.front();
	lstdev.erase(lstdev.begin());
	strport = lstdev.front();
	lstdev.erase(lstdev.begin());
	int pardev = CFITOM::GetDeviceIDFromName(strdevname.c_str());
	CPort* pt;
	CPort* pt2 = 0;
	if (pt = CreatePort(pardev, strport.c_str())) {
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

int CFITOMConfig::CreateSingleDevice(int devtype, LPCTSTR param)
{
	int res = 0;
	int md;
	std::tstring strparam(param);
	std::tstring strport;
	std::vector<std::tstring > lstparam;
	boost::split(lstparam, strparam, boost::is_any_of(_T(",")));
	if (lstparam.size() < 2) { return -1; }
	md = std::stoi(lstparam.front());
	lstparam.erase(lstparam.begin());
	strport = lstparam.front();
	lstparam.erase(lstparam.begin());
	CPort* pt = 0;
	CPort* pt2 = 0;
	if ((pt = CreatePort(devtype, strport.c_str()))) {
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
			AddDevice(new CSSG(pt, fs/2));
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
			std::strncpy(name, pcmprog.progname, count);
			ret = std::strlen(name);
		}
	}
	else if (devid == RHYTHM_BANK) {
		if (prog < MAX_BANK && vDrumBank[prog]) {
			std::strncpy(name, vDrumBank[prog]->GetBankName(), count);
			ret = std::strlen(name);
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
			std::strncpy(name, vname, count);
			ret = std::strlen(name);
		}
	}
	if (ret == 0) {
		std::strncpy(name, _T("--------"), count);
		ret = std::strlen(name);
	}
	return ret;
}

int CFITOMConfig::GetDeviceName(UINT32 devid, TCHAR* name, size_t count)
{
	int ret = 0;
	std::tstring res(_T(""));
	const TCHAR* devname = CFITOM::GetDeviceNameFromID(devid & 0xff);
	if (isPcmDevice(devid)) {
		res = (boost::format(_T("%1%(%2%)")) % std::tstring(devname) % CFITOM::GetDeviceNameFromID(devid >> 24)).str();
	}
	else {
		res = std::tstring(devname);
	}
	if (res.compare(_T("")) != 0) {
		std::strncpy(name, res.c_str(), count);
	}
	return ret;
}

int CFITOMConfig::LoadConfig()
{
	pProgressMessage ? pProgressMessage(_T("Loading Device Setting...")) : void(0);
	std::terr << LoadDeviceConfig() << _T(" SCCI Devices configured.") << std::endl;
	pProgressMessage ? pProgressMessage(_T("Loading ADPCM Setting...")) : void(0);
	std::terr << LoadADPCMConfig() << _T(" ADPCM Devices configured.") << std::endl;
	pProgressMessage ? pProgressMessage(_T("Loading MIDI Setting...")) : void(0);
	std::terr << LoadMidiConfig() << _T(" MIDI IN ports configured.") << std::endl;
	pProgressMessage ? pProgressMessage(_T("Loading Voice Setting...")) : void(0);
	LoadVoiceConfig();
	pProgressMessage ? pProgressMessage(_T(" ")) : void(0);
	return 0;
}

int CFITOMConfig::LoadMidiConfig()
{
	//MIDI Input
	int res = 0;
	int clockrx = fitom_ini.get<int>(_T("MIDI.ClockRx"), 0);
	SetMIDIClockCh(clockrx);
	for (int i = 0; i < MAX_MPUS; i++) {
		std::tstring strkeyname;
		std::tstring strportname;
		strkeyname = (boost::format(_T("MIDI.MIDIIN%1%")) % (i + 1)).str();
		strportname = fitom_ini.get<std::tstring>(strkeyname, _T("**NONE**"));
		CMidiIn* midiport = 0;
		if (strportname.compare(_T("**NONE**"))) {
			midiport = CreateMidiInPort(strportname.c_str());
			if (midiport) {
				AddDevice(midiport);
				res++;
				for (int j = 0; j < 16; j++) {
					std::tstring strchkey;
					std::tstring strparam;
					strchkey = (boost::format(_T("Channel.ch%1%")) % (j + 1)).str();
					strparam = fitom_ini.get<std::tstring>(strchkey, _T("**NONE**"));
					if (strparam.compare(_T("**NONE**"))) {
						if (strparam.compare(_T("RHYTHM"))) {
							std::vector<std::tstring> lstdevparam;
							boost::split(lstdevparam, strparam, boost::is_any_of(_T(":")));
							if (lstdevparam.size()) {
								int pol = 0;
								if (lstdevparam.size() > 1) {
									pol = std::stoi(lstdevparam[1]);
								}
								int devid = CFITOM::GetDeviceIDFromName(lstdevparam[0].c_str());
								SetChannelMap(j, devid, pol);

							}
						}
						else {
							SetChannelMap(j, DEVICE_RHYTHM, 0);
						}
					}
				}
			}
		}
	}
	return res;
}

int CFITOMConfig::LoadDeviceConfig()
{
	//SCCI devices
	int res = 0;
	int devices = fitom_ini.get<int>(_T("Device.count"), 0);
	for (int i = 0; i < devices; i++) {
		std::tstring strkeyname = (boost::format(_T("Device.device%1%")) % i).str();
		boost::optional<std::tstring> valparam = fitom_ini.get_optional<std::tstring>(strkeyname);
		if (valparam && CreateDevice(valparam.get().c_str()) == 0) {
			res++;
		}
	}
	return res;
}

int CFITOMConfig::LoadVoiceConfig()
{
	std::terr << ParseVoiceBank(VOICE_GROUP_OPM) << _T(" OPM Banks configured.") << std::endl;
	std::terr << ParseVoiceBank(VOICE_GROUP_OPNA) << _T(" OPN Banks configured.") << std::endl;
	std::terr << ParseVoiceBank(VOICE_GROUP_OPL2) << _T(" OPL2 Banks configured.") << std::endl;
	std::terr << ParseVoiceBank(VOICE_GROUP_OPL3) << _T(" OPL3 Banks configured.") << std::endl;
	std::terr << ParseVoiceBank(VOICE_GROUP_OPLL) << _T(" OPLL Banks configured.") << std::endl;
	std::terr << ParseVoiceBank(VOICE_GROUP_PSG) << _T(" PSG Banks configured.") << std::endl;
	std::terr << ParseRhythmBank() << _T(" Rhythm Banks configured.") << std::endl;
	return 0;
}

int CFITOMConfig::LoadADPCMConfig()
{
	//SCCI devices
	int res = 0;
	int devices = fitom_ini.get<int>(_T("ADPCM.Count"), 0);
	for (int i = 0; i < devices; i++) {
		std::tstring strkeyname;
		std::tstring strparam;
		strkeyname = (boost::format(_T("ADPCM.device%1%")) % i).str();
		strparam = fitom_ini.get<std::tstring>(strkeyname, _T("**NONE**"));
		if (strparam.compare(_T("**NONE**"))) {
			std::tstring strdev;
			std::tstring strport;
			std::tstring strbankfile;
			std::vector<std::tstring> lstparam;
			boost::split(lstparam, strparam, boost::is_any_of(_T(",")));
			if (lstparam.size() > 2) {
				strdev = lstparam[0];
				strport = lstparam[1];
				strbankfile = lstparam[2];
				strparam = (boost::format(_T("%1%,%2%")) % strdev % strport).str();
				boost::system::error_code err;
				bool exists = boost::filesystem::exists(boost::filesystem::path(strbankfile), err);
				if (exists && !err && CreateDevice(strparam.c_str()) == 0) {
					std::terr << LoadADPCMBank(i, strbankfile.c_str()) << _T(" files loaded for ") << strparam;
					res++;
				}
			}
		}
	}
	return res;
}

int CFITOMConfig::LoadADPCMBank(int bank, LPCTSTR fname)
{
	int res = 0;
	CAdPcmBase* pdev = GetPCMDeviceFromIndex(bank);
	if (pdev) {
		CPcmBank* pbank = AllocPcmBank(bank);
		std::tstring strbankname;
		boost::property_tree::ptree bnkfile;
		boost::property_tree::read_ini(fname, bnkfile);
		strbankname = (bnkfile.get<std::tstring>(_T("Header.BankName"), _T("**NONE**")));
		pbank->SetBankName(strbankname.c_str());
		for (int i = 0; i < 128; i++) {
			std::tstring strkeyname;
			strkeyname = (boost::format(_T("Bank.prog%1%")) % i).str();
			std::tstring strparam = bnkfile.get<std::tstring>(strkeyname, _T("**NONE**"));
			if (strparam.compare(_T("**NONE**"))) {
				std::vector<std::tstring> lstparam;
				boost::split(lstparam, strparam, boost::is_any_of(_T(",")));
				if (lstparam.size() > 1) {
					std::tstring strwavfile = lstparam[0];
					int hirate = std::stoi(lstparam[1]);
					boost::system::error_code err;
					bool exists = boost::filesystem::exists(strwavfile, err);
					if (exists && !err) {
						pProgressFilename ? pProgressFilename(strwavfile.c_str()) : void(0);
						std::terr << _T("Loading: ") << strwavfile << _T("...") << std::endl;
						Adpcm adpcm;
						size_t length = boost::filesystem::file_size(boost::filesystem::path(strwavfile), err);
						std::ifstream infile(strwavfile, std::ios::in | std::ios::binary);
						LPBYTE pdata = new BYTE[length];
						infile.read((char*)pdata, length);
						std::terr << _T(" loaded: ") << length << _T(" -->");
						DWORD ressize = 0;
						LPBYTE presult = adpcm.waveToAdpcm(pdata, (DWORD)length, ressize, hirate);
						std::terr << _T(" encoded: ") << ressize << _T(" -->");
						pdev->LoadVoice(i, presult, ressize);
						res++;
						std::terr << _T("wrote to device.") << std::endl;
						strwavfile = boost::filesystem::path(strwavfile).stem().generic_tstring();
						PCMPROG pcmprog;
						std::strncpy(pcmprog.progname, strwavfile.c_str(), (sizeof(PCMPROG::progname) / sizeof(TCHAR)) - 1);
						pcmprog.hirate = hirate;
						pbank->SetVoice(i, &pcmprog);
						delete[] pdata;
						delete[] presult;
					}
					else {
						std::terr << _T("File not found: ") << strwavfile << std::endl;
					}
				}
			}
		}
	}
	return res;
}

int CFITOMConfig::ParseRhythmBank()
{
	int res = 0;
	for (int i = 0; i < MAX_BANK; i++) {
		std::tstring strkeyname;
		std::tstring strbankfile;
		strkeyname = (boost::format(_T("DRUM.prog%1%")) % i).str();
		boost::property_tree::ptree bankfile;
		strbankfile = fitom_ini.get<std::tstring>(strkeyname, _T("**NONE**"));
		boost::system::error_code err;
		bool exists = boost::filesystem::exists(boost::filesystem::path(strbankfile), err);
		if (exists && !err) {
			pProgressFilename ? pProgressFilename(strbankfile.c_str()) : void(0);
			CDrumBank* bank = AllocDrumBank(i);
			if (bank) {
				if (!LoadDrumBank(bank, strbankfile.c_str())) {
					res++;
				}
			}
		}
	}
	return res;
}

int CFITOMConfig::LoadDrumBank(CDrumBank* bank, LPCTSTR fname)
{
	if (fname) {
		bank->SetFileName(fname);
	}

	int ret = 0;
	boost::system::error_code err;
	bool exists = boost::filesystem::exists(boost::filesystem::path(fname), err);
	if (exists && !err) {
		boost::property_tree::ptree bankfile;
		std::tstring strtmp;
		boost::property_tree::read_ini(fname, bankfile);
		strtmp = bankfile.get<std::tstring>(_T("Header.Type"), _T("**NONE**"));
		if (strtmp.compare(_T("RHYTHM"))) { return -1; }
		strtmp = bankfile.get<std::tstring>(_T("Header.BankName"), _T("**NONE**"));
		if (strtmp.compare(_T("**NONE**"))) { bank->SetBankName(strtmp.c_str()); }
		for (int note = 0; note < 128; note++) {
			std::tstring strkeyname = (boost::format(_T("Bank.Note%1%")) % note).str();
			strtmp = bankfile.get<std::tstring>(strkeyname, _T("**NONE**"));
			if (strtmp.compare(_T("**NONE**"))) {
				int prog = 0;
				int pan = 0;
				int gate = 0;
				int bkno = 0;
				int num = 0;
				int fnum = 0;
				std::tstring strdevname;
				std::tstring strnotename;
				std::tstring strnote;
				DRUMMAP drumnote;
				memset(&drumnote, 0, sizeof(DRUMMAP));
				std::vector<std::tstring> lstparam;
				boost::split(lstparam, strtmp, boost::is_any_of(_T(",")));
				if (lstparam.size() > 6) {
					strnotename = lstparam[0];
					strdevname = lstparam[1];
					bkno = std::stoi(lstparam[2]);
					prog = std::stoi(lstparam[3]);
					strnote = lstparam[4];
					pan = std::stoi(lstparam[5]);
					gate = std::stoi(lstparam[6]);
					if (strnote[0] == _T('#')) {
						std::vector<std::tstring> lstnote;
						strnote = strnote.substr(1);
						boost::split(lstnote, strnote, boost::is_any_of(_T(":")));
						if (lstnote.size() > 1) {
							num = std::stoi(lstnote[0]) | 0x80;
							fnum = std::stoi(lstnote[1], 0, 16);
						}
					}
					else {
						std::vector<std::tstring> lstnote;
						boost::split(lstnote, strnote, boost::is_any_of(_T(":")));
						if (lstnote.size() > 1) {
							num = std::stoi(lstnote[0]) | 0x80;
							fnum = std::stoi(lstnote[1], 0, 16);
						}
					}
					std::strncpy(drumnote.name, strnote.c_str(), (sizeof(DRUMMAP::name) / sizeof(TCHAR)) - 1);
					if (strdevname.find(_T(':')) > 0) {
						int ifid, slid;
						std::vector<std::tstring> lstaddparam;
						boost::split(lstaddparam, strdevname, boost::is_any_of(_T(":")));
						if (lstaddparam.size() > 1) {
							UINT8 devid = CFITOM::GetDeviceIDFromName(lstaddparam[0].c_str());
							if (CFITOM::GetDeviceVoiceGroupMask(devid) == VOICE_GROUP_PCM) {
								int adpcmidx = std::stoi(lstaddparam[1]);
								drumnote.device = GetPCMDeviceFromIndex(adpcmidx);
								if (drumnote.device) {
									drumnote.devID = GetDeviceUniqID((CAdPcmBase*)drumnote.device);
								}
							}
							else if (lstaddparam.size() > 2) {
								ifid = std::stoi(lstaddparam[1]);
								slid = std::stoi(lstaddparam[2]);
								drumnote.devID = (ifid << 16) | (slid << 8) | devid;
								drumnote.device = GetPhysDeviceFromID(drumnote.devID);
							}
						}
					}
					else {
						UINT8 devid = CFITOM::GetDeviceIDFromName(strdevname.c_str());
						if (CFITOM::GetDeviceVoiceGroupMask(devid) == VOICE_GROUP_PCM) {
							drumnote.device = GetPCMDeviceFromIndex(bkno);
							drumnote.devID = GetDeviceUniqID((CAdPcmBase*)drumnote.device);
						}
						else {
							drumnote.device = GetLogDeviceFromID(devid);
							drumnote.devID = GetDeviceUniqID(drumnote.device);
						}
					}
					drumnote.bank = UINT8(bkno);
					drumnote.prog = UINT8(prog);
					drumnote.pan = SINT8(pan);
					drumnote.gate = UINT16(gate);
					drumnote.num = UINT8(num);
					drumnote.fnum = UINT16(fnum);
					bank->SetVoice(note, &drumnote);
				}
			}
		}
	}
	else {
		std::terr << _T("Error loading \"") << fname << _T("\"") << std::endl;
		ret = -1;
	}
	return ret;
}


int CFITOMConfig::ParseVoiceBank(int groupcode)
{
	LPCTSTR groupname;
	int res = 0;
	int(CFITOMConfig::*parseFunc)(FMVOICE*, int, std::vector<int>&) = 0;
	switch (groupcode) {
	case VOICE_GROUP_OPM:
		parseFunc = &CFITOMConfig::ParseOPMVoice;
		groupname = _T("OPM");
		break;
	case VOICE_GROUP_OPNA:
		parseFunc = &CFITOMConfig::ParseOPNVoice;
		groupname = _T("OPNA");
		break;
	case VOICE_GROUP_OPL2:
		parseFunc = &CFITOMConfig::ParseOPL2Voice;
		groupname = _T("OPL2");
		break;
	case VOICE_GROUP_OPL3:
		parseFunc = &CFITOMConfig::ParseOPL3Voice;
		groupname = _T("OPL3");
		break;
	case VOICE_GROUP_OPLL:
		parseFunc = &CFITOMConfig::ParseOPLLVoice;
		groupname = _T("OPLL");
		break;
	case VOICE_GROUP_PSG:
		parseFunc = &CFITOMConfig::ParsePSGVoice;
		groupname = _T("PSG");
		break;
	default:
		return res;
	}
	for (int i = 0; i < MAX_BANK; i++) {
		std::tstring strkeyname;
		std::tstring strbankfile;
		strkeyname = (boost::format(_T("%1%.bank%2%")) % groupname % i).str();
		strbankfile = fitom_ini.get<std::tstring>(strkeyname, _T(""));
		boost::system::error_code err;
		bool exists = boost::filesystem::exists(boost::filesystem::path(strbankfile), err);
		if (exists && !err) {
			pProgressFilename ? pProgressFilename(strbankfile.c_str()) : void();
			CFMBank* bank = AllocFMBank(groupcode, i);
			if (bank) {
				std::tstring strbankname;
				std::tstring strprofval;
				boost::property_tree::ptree bankfile;
				boost::property_tree::read_ini(strbankfile, bankfile);
				strprofval = bankfile.get<std::tstring>(_T("Header.BankName"), _T("**NONE**"));
				bank->SetFileName(strbankfile.c_str());
				if (strprofval.compare(_T("**NONE**"))) {
					bank->SetBankName(strprofval.c_str());
				}
				for (int j = 0; j < 128; j++) {
					std::tstring strprogkey;
					strprogkey = (boost::format(_T("Prog%1%")) % j).str();
					FMVOICE voice;
					std::memset(&voice, 0, sizeof(FMVOICE));
					std::tstring strprogname;
					std::tstring stropparam;
					strprogname = bankfile.get<std::tstring>(strprogkey + std::tstring(_T(".Name")), _T("**NONE**"));
					std::strncpy(voice.name, strprogname.c_str(), 15);
					std::vector<int> opparam;
					std::vector<std::tstring> lstopparam;
					stropparam = bankfile.get<std::tstring>(strprogkey + std::tstring(_T(".ALFB")), _T("0 0 0 0"));
					boost::trim(stropparam);
					boost::split(lstopparam, stropparam, boost::is_space());
					BOOST_FOREACH(std::tstring s, lstopparam) { if (s != _T("")) opparam.push_back(stoi(s)); }
					(this->*parseFunc)(&voice, 0, opparam);
					lstopparam.clear();
					opparam.clear();
					stropparam = bankfile.get<std::tstring>(strprogkey + std::tstring(_T(".LFO0")), _T("0 0 0 0 0"));
					boost::trim(stropparam);
					boost::split(lstopparam, stropparam, boost::is_space());
					BOOST_FOREACH(std::tstring s, lstopparam) { if (s != _T("")) opparam.push_back(stoi(s)); }
					ParseLFOParam(&voice, 0, opparam);
					for (int k = 0; k < 4; k++) {
						std::tstring stropkey = (boost::format(_T("%1%.OP%2%")) % strprogkey % (k + 1)).str();
						lstopparam.clear();
						opparam.clear();
						stropparam = bankfile.get<std::tstring>(stropkey, _T("0 0 0 0 0 0 0 0 0 0 0"));
						boost::trim(stropparam);
						boost::split(lstopparam, stropparam, boost::is_space());
						BOOST_FOREACH(std::tstring s, lstopparam) { if (s != _T("")) opparam.push_back(stoi(s)); }
						(this->*parseFunc)(&voice, k + 1, opparam);
						stropkey = (boost::format(_T("%1%.LFO%2%")) % strprogkey % (k + 1)).str();
						lstopparam.clear();
						opparam.clear();
						stropparam = bankfile.get<std::tstring>(stropkey, _T("0 0 0 0 0"));
						boost::trim(stropparam);
						boost::split(lstopparam, stropparam, boost::is_space());
						BOOST_FOREACH(std::tstring s, lstopparam) { if (s != _T("")) opparam.push_back(stoi(s)); }
						ParseLFOParam(&voice, k + 1, opparam);
					}
					voice.ID = (groupcode << 24) | (i << 8) | j;
					bank->SetVoice(j, &voice);
				}
				res++;
			}
		}
	}
	return res;
}

int CFITOMConfig::ParseOPMVoice(FMVOICE* voice, int index, std::vector<int>& param)
{
	int k = index - 1;
	if (0 <= k && k < 4 && param.size() > 10) {
		voice->op[k].AR = (param[0] & 31) << 2;
		voice->op[k].DR = (param[1] & 31) << 2;
		voice->op[k].SR = (param[2] & 31) << 2;
		voice->op[k].RR = (param[3] & 15) << 3;
		voice->op[k].SL = (param[4] & 15) << 3;
		voice->op[k].TL = (param[5] & 127);
		voice->op[k].KSL = (param[6] & 3);
		voice->op[k].MUL = (param[7] & 15);
		voice->op[k].DT1 = (param[8] & 7);
		voice->op[k].DT2 = (param[9] & 3);
		voice->op[k].AM = (param[10] & 1);
		voice->op[k].REV = 4;
	}
	else if (index == 0 && param.size() > 3) {
		voice->AL = (param[0] & 7) | ((param[2] & 1) << 3);
		voice->FB = param[1] & 7;
		voice->NFQ = param[3] & 31;
	}
	return 0;
}

int CFITOMConfig::ParseOPNVoice(FMVOICE* voice, int index, std::vector<int>& param)
{
	int k = index - 1;
	if (0 <= k && k < 4 && param.size() > 10) {
		voice->op[k].AR = (param[0] & 31) << 2;
		voice->op[k].DR = (param[1] & 31) << 2;
		voice->op[k].SR = (param[2] & 31) << 2;
		voice->op[k].RR = (param[3] & 15) << 3;
		voice->op[k].SL = (param[4] & 15) << 3;
		voice->op[k].TL = (param[5] & 127);
		voice->op[k].KSL = (param[6] & 3);
		voice->op[k].MUL = (param[7] & 15);
		voice->op[k].DT1 = (param[8] & 7);
		voice->op[k].EGT = (param[9] & 15);
		voice->op[k].AM = (param[10] & 1);
		voice->op[k].REV = 32;
	}
	else if (index == 0 && param.size() > 1) {
		voice->AL = (param[0] & 7);
		voice->FB = param[1] & 7;
	}
	return 0;
}

int CFITOMConfig::ParseOPL2Voice(FMVOICE* voice, int index, std::vector<int>& param)
{
	int k = index - 1;
	if (0 <= k && k < 2 && param.size() > 10) {
		voice->op[k].AR = (param[0] & 15) << 3;
		voice->op[k].DR = (param[1] & 15) << 3;
		voice->op[k].SR = (param[2] & 15) << 3;
		voice->op[k].RR = (param[3] & 15) << 3;
		voice->op[k].SL = (param[4] & 15) << 3;
		voice->op[k].TL = (param[5] & 63);
		voice->op[k].KSL = (param[6] & 7) >> 1;
		voice->op[k].KSR = (param[6] & 1);
		voice->op[k].MUL = (param[7] & 15);
		voice->op[k].DT1 = (param[8] & 0x3fff) >> 7;
		voice->op[k].DT2 = (param[8] & 0x7f);
		voice->op[k].WS = (param[9] & 7);
		voice->op[k].VIB = (param[10] >> 1) & 1;
		voice->op[k].AM = (param[10] & 1);
		voice->op[k].REV = 32;
	}
	else if (index == 0 && param.size() > 1) {
		voice->AL = (param[0] & 7);
		voice->FB = param[1] & 7;
	}
	return 0;
}

int CFITOMConfig::ParseOPL3Voice(FMVOICE* voice, int index, std::vector<int>& param)
{
	int k = index - 1;
	if (0 <= k && k < 4 && param.size() > 10) {
		voice->op[k].AR = (param[0] & 15) << 3;
		voice->op[k].DR = (param[1] & 15) << 3;
		voice->op[k].SR = (param[2] & 15) << 3;
		voice->op[k].RR = (param[3] & 15) << 3;
		voice->op[k].SL = (param[4] & 15) << 3;
		voice->op[k].TL = (param[5] & 63);
		voice->op[k].KSL = (param[6] & 7) >> 1;
		voice->op[k].KSR = (param[6] & 1);
		voice->op[k].MUL = (param[7] & 15);
		voice->op[k].DT1 = (param[8] & 0x3fff) >> 7;
		voice->op[k].DT2 = (param[8] & 0x7f);
		voice->op[k].WS = (param[9] & 7);
		voice->op[k].VIB = (param[10] >> 1) & 1;
		voice->op[k].AM = (param[10] & 1);
		voice->op[k].REV = 32;
	}
	else if (index == 0 && param.size() > 2) {
		voice->AL = (param[0] & 15);
		voice->FB = (param[1] & 7) | ((param[2] & 7) << 3);
	}
	return 0;
}

int CFITOMConfig::ParseOPLLVoice(FMVOICE* voice, int index, std::vector<int>& param)
{
	int k = index - 1;
	if (0 <= k && k < 2 && param.size() > 10) {
		voice->op[k].AR = (param[0] & 15) << 3;
		voice->op[k].DR = (param[1] & 15) << 3;
		voice->op[k].SR = (param[2] & 15) << 3;
		voice->op[k].RR = (param[3] & 15) << 3;
		voice->op[k].SL = (param[4] & 15) << 3;
		voice->op[k].TL = (param[5] & 63);
		voice->op[k].KSL = (param[6] & 7) >> 1;
		voice->op[k].KSR = (param[6] & 1);
		voice->op[k].MUL = (param[7] & 15);
		voice->op[k].DT1 = (param[8] & 0x3fff) >> 7;
		voice->op[k].DT2 = (param[8] & 0x7f);
		voice->op[k].WS = (param[9] & 7);
		voice->op[k].VIB = (param[10] >> 1) & 1;
		voice->op[k].AM = (param[10] & 1);
		voice->op[k].REV = 32;
	}
	else if (index == 0) {
		if (param.size() == 1 && param[0]) {
			voice->AL = 0x40 | (param[0] & 15);
		}
		else if (param.size() > 1) {
			voice->AL = 0;
			voice->FB = param[1] & 7;
		}
	}
	return 0;
}

int CFITOMConfig::ParsePSGVoice(FMVOICE* voice, int index, std::vector<int>& param)
{
	int k = index - 1;
	if (k == 0 && param.size() > 9) {
		voice->op[k].AR = (param[0] & 127);
		voice->op[k].DR = (param[1] & 127);
		voice->op[k].SR = (param[2] & 127);
		voice->op[k].RR = (param[3] & 127);
		voice->op[k].SL = (param[4] & 127);
		voice->op[k].EGS = (param[5] & 127);
		voice->op[k].WS = (param[9] & 7);
		voice->op[k].REV = 32;
	}
	else if (index == 0 && param.size() > 1) {
		voice->AL = param[0] & 3;
		voice->NFQ = param[1] & 31;
	}
	return 0;
}

int CFITOMConfig::ParseLFOParam(FMVOICE* voice, int index, std::vector<int>& param)
{
	int k = index - 1;
	if (0 < k && k < 4 && param.size() > 4) {
		voice->op[k].SLW = param[0] & 7;
		voice->op[k].SLY = param[1] & 127;
		voice->op[k].SLR = param[2] & 127;
		voice->op[k].SLD = param[3] & 127;
		voice->op[k].SLF = param[4] & 15;
	}
	else if (index == 0 && param.size() > 4) {
		voice->LWF = param[0] & 7;
		voice->LFD = param[1] & 127;
		voice->LFR = param[2] & 127;
		voice->LDL = param[3] & 127;
		voice->LDM = (param[3] >> 7) & 127;
		voice->LFO = param[4] & 15;
	}
	return 0;
}

int CFITOMConfig::BuildLFOParam(FMVOICE* voice, int index, TCHAR* result, size_t length)
{
	std::tstring strres;
	if (index == 0) {
		int depth = (voice->LDM << 7) | voice->LDL;
		if (depth > 8191) { depth -= 16384; }
		strres = (boost::format(_T("%1% %2% %3% %4% %5%")) % voice->LWF % voice->LFD % voice->LFR % depth % voice->LFO).str();
		return std::strlen(std::strncpy(result, strres.c_str(), length - 1));
	}
	int k = index - 1;
	int depth = voice->op[k].SLD;
	if (depth > 63) { depth -= 128; }
	strres = (boost::format(_T("%1% %2% %3% %4% %5%")) % voice->op[k].SLW % voice->op[k].SLY % voice->op[k].SLR % depth % voice->op[k].SLF).str();
	return std::strlen(std::strncpy(result, strres.c_str(), length - 1));
}

int CFITOMConfig::BuildOPMVoice(FMVOICE* voice, int index, TCHAR* result, size_t length)
{
	std::tstring strres;
	if (index == 0) {
		strres = (boost::format(_T("%1% %2% %3% %4%")) % (voice->AL & 7) % voice->FB % ((voice->AL & 8) >> 3) % voice->NFQ).str();
		return std::strlen(std::strncpy(result, strres.c_str(), length - 1));
	}
	int k = index - 1;
	strres = (boost::format(_T("%1% %2% %3% %4% %5% %6% %7% %8% %9% %10% %11%"))
		% (voice->op[k].AR >> 2) % (voice->op[k].DR >> 2) % (voice->op[k].SR >> 2) % (voice->op[k].RR >> 3) % (voice->op[k].SL >> 3)
		% voice->op[k].TL % voice->op[k].KSL % voice->op[k].MUL % voice->op[k].DT1 % voice->op[k].DT2 % voice->op[k].AM).str();
	return std::strlen(std::strncpy(result, strres.c_str(), length - 1));
}

int CFITOMConfig::BuildOPNVoice(FMVOICE* voice, int index, TCHAR* result, size_t length)
{
	std::tstring strres;
	if (index == 0) {
		strres = (boost::format(_T("%1% %2%")) % (voice->AL & 7) % voice->FB).str();
		return std::strlen(std::strncpy(result, strres.c_str(), length - 1));
	}
	int k = index - 1;
	strres = (boost::format(_T("%1% %2% %3% %4% %5% %6% %7% %8% %9% %10% %11%"))
		% (voice->op[k].AR >> 2) % (voice->op[k].DR >> 2) % (voice->op[k].SR >> 2) % (voice->op[k].RR >> 3) % (voice->op[k].SL >> 3)
		% voice->op[k].TL % voice->op[k].KSL % voice->op[k].MUL % voice->op[k].DT1 % voice->op[k].EGT % voice->op[k].AM).str();
	return std::strlen(std::strncpy(result, strres.c_str(), length - 1));
}

int CFITOMConfig::BuildOPL2Voice(FMVOICE* voice, int index, TCHAR* result, size_t length)
{
	std::tstring strres;
	if (index == 0) {
		strres = (boost::format(_T("%1% %2%")) % (voice->AL & 1) % voice->FB).str();
		return std::strlen(std::strncpy(result, strres.c_str(), length - 1));
	}
	int k = index - 1;
	int pdt = (voice->op[k].DT1 << 7) | voice->op[k].DT2;
	if (pdt > 8191) { pdt -= 16384; }
	strres = (boost::format(_T("%1% %2% %3% %4% %5% %6% %7% %8% %9% %10% %11%"))
		% (voice->op[k].AR >> 3) % (voice->op[k].DR >> 3) % (voice->op[k].SR >> 3) % (voice->op[k].RR >> 3) % (voice->op[k].SL >> 3)
		% (voice->op[k].TL >> 1) % voice->op[k].KSL % voice->op[k].MUL % pdt % voice->op[k].WS % ((voice->op[k].VIB << 1) | voice->op[k].AM)).str();
	return std::strlen(std::strncpy(result, strres.c_str(), length - 1));
}

int CFITOMConfig::BuildOPL3Voice(FMVOICE* voice, int index, TCHAR* result, size_t length)
{
	std::tstring strres;
	if (index == 0) {
		strres = (boost::format(_T("%1% %2% %3%")) % (voice->AL & 15) % (voice->FB & 7) % ((voice->FB >> 3) & 7)).str();
		return std::strlen(std::strncpy(result, strres.c_str(), length - 1));
	}
	int k = index - 1;
	int pdt = (voice->op[k].DT1 << 7) | voice->op[k].DT2;
	if (pdt > 8191) { pdt -= 16384; }
	strres = (boost::format(_T("%1% %2% %3% %4% %5% %6% %7% %8% %9% %10% %11%"))
		% (voice->op[k].AR >> 3) % (voice->op[k].DR >> 3) % (voice->op[k].SR >> 3) % (voice->op[k].RR >> 3) % (voice->op[k].SL >> 3)
		% (voice->op[k].TL >> 1) % voice->op[k].KSL % voice->op[k].MUL % pdt % voice->op[k].WS % ((voice->op[k].VIB << 1) | voice->op[k].AM)).str();
	return std::strlen(std::strncpy(result, strres.c_str(), length - 1));
}

int CFITOMConfig::BuildOPLLVoice(FMVOICE* voice, int index, TCHAR* result, size_t length)
{
	std::tstring strres;
	if (index == 0) {
		if (voice->AL & 0x40) {
			strres = (boost::format(_T("%1%")) % (voice->AL & 15)).str();
			return std::strlen(std::strncpy(result, strres.c_str(), length - 1));
		}
		else {
			strres = (boost::format(_T("%1% %2%")) % (voice->AL & 1) % voice->FB).str();
			return std::strlen(std::strncpy(result, strres.c_str(), length - 1));
		}
	}
	int k = index - 1;
	int pdt = (voice->op[k].DT1 << 7) | voice->op[k].DT2;
	if (pdt > 8191) { pdt -= 16384; }
	strres = (boost::format(_T("%1% %2% %3% %4% %5% %6% %7% %8% %9% %10% %11%"))
		% (voice->op[k].AR >> 3) % (voice->op[k].DR >> 3) % (voice->op[k].SR >> 3) % (voice->op[k].RR >> 3) % (voice->op[k].SL >> 3)
		% (voice->op[k].TL >> 1) % voice->op[k].KSL % voice->op[k].MUL % pdt % voice->op[k].WS % ((voice->op[k].VIB << 1) | voice->op[k].AM)).str();
	return std::strlen(std::strncpy(result, strres.c_str(), length - 1));
}

int CFITOMConfig::BuildPSGVoice(FMVOICE* voice, int index, TCHAR* result, size_t length)
{
	std::tstring strres;
	if (index == 0) {
		strres = (boost::format(_T("%1%")) % voice->AL % (voice->NFQ & 7)).str();
		return std::strlen(std::strncpy(result, strres.c_str(), length - 1));
	}
	int k = index - 1;
	strres = (boost::format(_T("%1% %2% %3% %4% %5% %6% %7% %8% %9% %10% %11%"))
		% voice->op[k].AR % voice->op[k].DR % voice->op[k].SR % voice->op[k].RR % voice->op[k].SL
		% voice->op[k].EGS % voice->op[k].EGT % 0 % 0 % voice->op[k].WS % 0).str();
	return std::strlen(std::strncpy(result, strres.c_str(), length - 1));
}
