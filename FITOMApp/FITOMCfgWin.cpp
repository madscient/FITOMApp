#include "stdafx.h"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include "FITOM.h"
#include "SoundDev.h"
#include "ADPCM.h"
#include "FITOMCfg.h"
#include "MIDIDEV.h"
#include "SCCIWrapper.h"
#include "Port.h"
#include "MIDI.h"
#include "MasVolWin.h"
#include "FTSPI.h"
#include "YMDeltaT.h"
#include "PCMD8.h"
#include "MAx.h"

// CFITOMConfigWin32

CFITOMConfigWin32::CFITOMConfigWin32(LPCTSTR strinifile) : CFITOMConfig(strinifile), g_devno(0), pScci(new CSCCIWrapper()), pFtspi(new CFTSPI())
{
}

CFITOMConfigWin32::~CFITOMConfigWin32()
{
	delete pScci;
	delete pFtspi;
}

CPort* CFITOMConfigWin32::CreatePort(int devtype, LPCTSTR params)
{
	PortInfo pinf = { 0, 0, };
	std::tstring strparams(params);
	std::tstring striftype;
	std::tstring strportdef;
	std::vector<std::tstring> lstparams;
	boost::split(lstparams, strparams, boost::is_any_of(_T(":")));
	if (lstparams.size() < 2) { return 0; }
	striftype = boost::trim_copy<std::tstring>(lstparams.front());
	lstparams.erase(lstparams.begin());
	tcscpy(pinf.ifname, striftype.c_str());

	if (striftype.compare(_T("SCCI")) == 0) {
		SC_CHIP_TYPE chiptype = pScci->getScChipType(devtype);
		strportdef = boost::trim_copy<std::tstring>(lstparams.front());
		if (strportdef.compare(_T("AUTO")) == 0) {
			lstparams.erase(lstparams.begin());
			SoundChip* chip = pScci->getSoundChipFromType(chiptype);
			if (chip) {
				SCCI_SOUND_CHIP_INFO* sci = chip->getSoundChipInfo();
				pinf.ifid = pScci->getInterfaceIDFromChip(chip);
				scciInterface* psi = pScci->getScciInterface(pinf.ifid);
				pinf.port = FindPort(pinf);
				if (pinf.port) {
					return pinf.port;
				}
				else {
					pinf.port = new CSCCIPort(psi, chip, size_t(CFITOM::GetDeviceRegSize(devtype)));
				}
				pinf.slid = pScci->getSlotIDFromChip(pinf.ifid, chip);
			}
		}
		else {
			if (lstparams.size() < 2) { return 0; }
			pinf.ifid = std::stoi(lstparams.front());
			lstparams.erase(lstparams.begin());
			pinf.slid = std::stoi(lstparams.front());
			lstparams.erase(lstparams.begin());
			SoundChip* chip = pScci->getSoundChipFromId(pinf.ifid, pinf.slid);
			scciInterface* psi = pScci->getScciInterface(pinf.ifid);
			if (chip && psi) {
				pinf.port = FindPort(pinf);
				if (pinf.port) {
					return pinf.port;
				}
				else {
					pinf.port = new CSCCIPort(psi, chip, size_t(CFITOM::GetDeviceRegSize(devtype)));
				}
			}
		}
	}
	if (striftype.compare(_T("FTSPI")) == 0 && pFtspi->IsValid()) {
		strportdef = boost::trim_copy<std::tstring>(lstparams.front());
		if (lstparams.size() < 2) { return 0; }
		pinf.ifid = std::stoi(lstparams.front());	//MPSSE_SPI CH No.
		lstparams.erase(lstparams.begin());
		pinf.slid = std::stoi(lstparams.front());	//MPSSE_SPI CS No.
		lstparams.erase(lstparams.begin());
		pinf.port = FindPort(pinf);
		if (pinf.port) {
			return pinf.port;
		}
		else {
			pinf.port = new CFTSPIPort(pFtspi, pinf.ifid, pinf.slid, size_t(CFITOM::GetDeviceRegSize(devtype)));
		}
	}
	if (!pinf.port) {
		fprintf(stderr, _T("Failed to init port: %s\n"), params);
	}
	if (pinf.port) {
		pinf.port->SetPhysicalId((pinf.ifid << 16) | (pinf.slid << 8) | devtype);
		vPort.push_back(pinf);
	}
	return pinf.port;
}

CPort* CFITOMConfigWin32::CreateSCCIPort(int ifid, int slid, int devtype)
{
	PortInfo pinf = { {_T("SCCI")}, ifid, slid, 0, };
	SoundChip* chip = pScci->getSoundChipFromId(pinf.ifid, pinf.slid);
	scciInterface* psi = pScci->getScciInterface(pinf.ifid);
	if (chip && psi) {
		pinf.port = FindPort(pinf);
		if (pinf.port) {
			return pinf.port;
		}
		else {
			pinf.port = new CSCCIPort(psi, chip, size_t(CFITOM::GetDeviceRegSize(devtype)));
		}
	}
	return pinf.port;
}
CMidiIn* CFITOMConfigWin32::CreateMidiInPort(LPCTSTR param)
{

	CMidiIn* ret = 0;
	try {
		ret = new CW32MidiIn(param);
	}
	catch (...) {

	}
	return ret;
}

CMasterVolumeCtrl* CFITOMConfigWin32::CreateMasVol(LPCTSTR param)
{
	CMasterVolumeCtrl* pctrl = new CMasVolWin32(param);
	if (pctrl->IsValid()) {
		return pctrl;
	}
	delete pctrl;
	pctrl = new CMasVolDummy();
	return pctrl;
}

void CFITOMConfigWin32::TimerCallBack()
{
	if (pFtspi->IsValid()) {
		//pFtspi->SPI_Flush();
	}
}

int CFITOMConfigWin32::AutoDeviceConfig()
{
	int res = 0;
	if (pScci) {
		for (int i = 0; i < pScci->getInterfaceCount(); i++) {
			for (int j = 0; j < pScci->getSoundChipCount(i); j++) {
				res += CreateSCCIDevice(i, j);
			}
		}
	}
	return res;
}

int CFITOMConfigWin32::CreateSCCIDevice(int ifid, int slid)
{
	int res = 0;
	SoundChip* psc = pScci->getSoundChipFromId(ifid, slid);
	SCCI_SOUND_CHIP_INFO* sci = psc->getSoundChipInfo();
	if (sci) {
		switch (sci->iSoundChip) {
		///OPMŒn
		case SC_CHIP_TYPE::SC_TYPE_YM2151:
		{
			CPort* pt = CreateSCCIPort(ifid, slid, DEVICE_OPM);
			if (pt) {
				res = AddOPM(pt, 0, sci->dClock);
			}
		}
		break;
		case SC_CHIP_TYPE::SC_TYPE_YM2164:
		{
			CPort* pt = CreateSCCIPort(ifid, slid, DEVICE_OPP);
			if (pt) {
				res = AddOPP(pt, 0, sci->dClock);
			}
		}
		break;
		case SC_CHIP_TYPE::SC_TYPE_YM2414:
		{
			CPort* pt = CreateSCCIPort(ifid, slid, DEVICE_OPZ);
			if (pt) {
				res = AddOPZ(pt, 0, sci->dClock);
			}
		}
		break;
		///OPNŒn
		case SC_CHIP_TYPE::SC_TYPE_YM2203:
		{
			CPort* pt = CreateSCCIPort(ifid, slid, DEVICE_OPN);
			if (pt) {
				res = AddOPN(pt, 0, sci->dClock);
				res += AddSSG(pt, 0, sci->dClock / 2);
			}
		}
		break;
		case SC_CHIP_TYPE::SC_TYPE_YM2608:
		{
			CPort* pt = CreateSCCIPort(ifid, slid, DEVICE_OPNA);
			COffsetPort* pt2 = new COffsetPort(pt, 0x100);
			if (pt && pt2) {
				res = AddOPNA(pt, 0, sci->dClock);
				res += AddSSGLP(pt, 0, sci->dClock / 2);
				res += AddOPNARhythm(pt, 0, sci->dClock);
				//if (AddDevice(new CAdPcm2608(pt2, sci->dClock, 256 * 1024))) {
				//	res++;
				//}
			}
		}
		break;
		case SC_CHIP_TYPE::SC_TYPE_YM2610:
		case SC_CHIP_TYPE::SC_TYPE_YMF286:
		{
			CPort* pt = CreateSCCIPort(ifid, slid, DEVICE_OPNB);
			COffsetPort* pt2 = new COffsetPort(pt, 0x100);
			if (pt && pt2) {
				res = AddOPNB(pt, 0, sci->dClock);
				res += AddSSG(pt, 0, sci->dClock / 2);
				if (AddDevice(new CAdPcm2610A(pt2, sci->dClock, 16384 * 1024))) {
					res++;
				}
				if (AddDevice(new CAdPcm2610B(pt, sci->dClock, 16384 * 1024))) {
					res++;
				}
			}
		}
		break;
		case SC_CHIP_TYPE::SC_TYPE_YM2610B:
		{
			CPort* pt = CreateSCCIPort(ifid, slid, DEVICE_OPNB);
			COffsetPort* pt2 = new COffsetPort(pt, 0x100);
			if (pt && pt2) {
				res = AddOPNA(pt, 0, sci->dClock);
				res += AddSSG(pt, 0, sci->dClock / 2);
				if (AddDevice(new CAdPcm2610A(pt2, sci->dClock, 16384 * 1024))) {
					res++;
				}
				if (AddDevice(new CAdPcm2610B(pt, sci->dClock, 16384 * 1024))) {
					res++;
				}
			}
		}
		break;
		case SC_CHIP_TYPE::SC_TYPE_YM2612:
		{
			CPort* pt = CreateSCCIPort(ifid, slid, DEVICE_OPN2);
			if (pt) {
				res = AddOPN2(pt, 0, sci->dClock);
			}
		}
		break;
		case SC_CHIP_TYPE::SC_TYPE_YMF276:
		{
			CPort* pt = CreateSCCIPort(ifid, slid, DEVICE_OPN2L);
			if (pt) {
				res = AddOPN3L(pt, 0, sci->dClock);
			}
		}
		break;
		case SC_CHIP_TYPE::SC_TYPE_YMF288:
		{
			CPort* pt = CreateSCCIPort(ifid, slid, DEVICE_OPN3L);
			COffsetPort* pt2 = new COffsetPort(pt, 0x100);
			if (pt && pt2) {
				res = AddOPN3L(pt, 0, sci->dClock);
				res += AddSSG(pt, 0, sci->dClock / 2);
				res += AddOPN3LRhythm(pt, 0, sci->dClock);
			}
		}
		break;
		///OPLŒn
		case SC_CHIP_TYPE::SC_TYPE_YM3526:
		{
			CPort* pt = CreateSCCIPort(ifid, slid, DEVICE_OPL);
			if (pt) {
				res = AddOPL(pt, 0, sci->dClock);
			}
		}
		break;
		case SC_CHIP_TYPE::SC_TYPE_YM3812:
		{
			CPort* pt = CreateSCCIPort(ifid, slid, DEVICE_OPL2);
			if (pt) {
				res = AddOPL2(pt, 0, sci->dClock);
			}
		}
		break;
		case SC_CHIP_TYPE::SC_TYPE_YMF262:
		{
			CPort* pt = CreateSCCIPort(ifid, slid, DEVICE_OPL3);
			if (pt) {
				res = AddOPL3(pt, 0, sci->dClock);
				res += AddOPL3_2(pt, 0, sci->dClock);
			}
		}
		break;
		case SC_CHIP_TYPE::SC_TYPE_Y8950:
		{
			CPort* pt = CreateSCCIPort(ifid, slid, DEVICE_Y8950);
			if (pt) {
				res = AddOPL(pt, 0, sci->dClock);
				if (AddDevice(new CAdPcm3801(pt, sci->dClock, 256 * 1024))) {
					res++;
				}
			}
		}
		///OPLLŒn
		case SC_CHIP_TYPE::SC_TYPE_YM2413:
		{
			CPort* pt = CreateSCCIPort(ifid, slid, DEVICE_OPLL);
			if (pt) {
				res = AddOPLL(pt, 0, sci->dClock);
			}
		}
		break;
		case SC_CHIP_TYPE::SC_TYPE_YM2420:
		{
			CPort* pt = CreateSCCIPort(ifid, slid, DEVICE_OPLL);
			if (pt) {
				res = AddOPLL2(pt, 0, sci->dClock);
			}
		}
		break;
		case SC_CHIP_TYPE::SC_TYPE_YM2423:
		{
			CPort* pt = CreateSCCIPort(ifid, slid, DEVICE_OPLL);
			if (pt) {
				res = AddOPLLX(pt, 0, sci->dClock);
			}
		}
		break;
		case SC_CHIP_TYPE::SC_TYPE_YMF281:
		{
			CPort* pt = CreateSCCIPort(ifid, slid, DEVICE_OPLL);
			if (pt) {
				res = AddOPLLP(pt, 0, sci->dClock);
			}
		}
		break;
		///PSGŒn
		case SC_CHIP_TYPE::SC_TYPE_AY8910:
		{
			CPort* pt = CreateSCCIPort(ifid, slid, DEVICE_PSG);
			if (pt) {
				res = AddPSG(pt, 0, sci->dClock);
			}
		}
		break;
		case SC_CHIP_TYPE::SC_TYPE_AY8930:
		{
			CPort* pt = CreateSCCIPort(ifid, slid, DEVICE_EPSG);
			if (pt) {
				res = AddEPSG(pt, 0, sci->dClock);
			}
		}
		break;
		case SC_CHIP_TYPE::SC_TYPE_YM2149:
		{
			CPort* pt = CreateSCCIPort(ifid, slid, DEVICE_SSG);
			if (pt) {
				res = AddSSG(pt, 0, sci->dClock);
			}
		}
		break;
		case SC_CHIP_TYPE::SC_TYPE_YMZ294:
		{
			CPort* pt = CreateSCCIPort(ifid, slid, DEVICE_SSG);
			if (pt) {
				res = AddSSGLP(pt, 0, sci->dClock);
			}
		}
		break;
		case SC_CHIP_TYPE::SC_TYPE_SN76489:
		{
			CPort* pt = CreateSCCIPort(ifid, slid, DEVICE_DCSG);
			if (pt) {
				res = AddDCSG(pt, 0, sci->dClock);
			}
		}
		break;
		case SC_CHIP_TYPE::SC_TYPE_SCC:
		{
			CPort* pt = CreateSCCIPort(ifid, slid, DEVICE_SCC);
			if (pt) {
				res = AddSCC(pt, 0, sci->dClock);
			}
		}
		break;
		case SC_CHIP_TYPE::SC_TYPE_SCCS:
		{
			CPort* pt = CreateSCCIPort(ifid, slid, DEVICE_SCCP);
			if (pt) {
				res = AddSCCP(pt, 0, sci->dClock);
			}
		}
		break;
		//•¡‡ƒfƒoƒCƒX
		case SC_CHIP_TYPE::SC_TYPE_NBV4:
		{
			CPort* pt = CreateSCCIPort(ifid, slid, 0x300);
			COffsetPort* pt2 = new COffsetPort(pt, 0x100);
			COffsetPort* pt3 = new COffsetPort(pt, 0x200);
			if (pt && pt2 && pt3) {
				res = AddOPM(pt, 0, sci->dClock);
				res += AddOPM(pt2, 0, sci->dClock);
				res += AddSSGLP(pt3, 0, sci->dClock / 2);
			}
		}
		break;
		case SC_CHIP_TYPE::SC_TYPE_AYB02:
		{
			CPort* pt = CreateSCCIPort(ifid, slid, 0x300);
			COffsetPort* pt2 = new COffsetPort(pt, 0x100);
			COffsetPort* pt3 = new COffsetPort(pt, 0x200);
			if (pt && pt2 && pt3) {
				res = AddOPLL(pt, 0, sci->dClock);
				res += AddOPLL(pt2, 0, sci->dClock);
				res += AddSSGLP(pt3, 0, sci->dClock / 2);
			}
		}
		break;
		}
	}
	return res;
}