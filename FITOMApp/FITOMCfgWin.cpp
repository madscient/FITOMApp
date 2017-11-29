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