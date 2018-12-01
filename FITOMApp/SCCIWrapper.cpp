#include "stdafx.h"
#include "scci.h"
#include "SCCIDefines.h"
#include <iostream>
#include "SCCIWrapper.h"
#include "FITOMdefine.h"

namespace ROM {
	struct DEVIDMAP {
		const UINT8 devid;
		const UINT8 voicegroup;
		const UINT8 voicetype;
		const DWORD regsize;
		const SC_CHIP_TYPE sctype;
		const char* chipname;
		const char* chipcode;
	};
	extern DEVIDMAP devmap[];
};

CSCCIWrapper::CSCCIWrapper()
{
	hScci = 0;
	pManager = 0;
	int res = Init();
}
int CSCCIWrapper::Init()
{
	// scci.dllの読込み
	if (!(hScci = ::LoadLibrary((LPCSTR)_T("scci.dll")))) {
		fprintf(stderr, _T("Failed to load scci.dll\n"));
		return ERROR_SCCI;
	}
	fprintf(stderr, _T("LoadLibrary...OK\n"));

	// サウンドインターフェースマネージャー取得用関数アドレス取得
	SCCIFUNC getSoundInterfaceManager = (SCCIFUNC)(::GetProcAddress(hScci, _T("getSoundInterfaceManager")));
	if (getSoundInterfaceManager == NULL){
		fprintf(stderr, _T("Failed to get interface\n"));
		return ERROR_SCCI;
	}
	fprintf(stderr, _T("GetProcAddress...OK\n"));
	// サウンドインターフェースマネージャー取得
	if (!(pManager = getSoundInterfaceManager())) {
		fprintf(stderr, _T("Failed to get Manager\n"));
		return ERROR_SCCI;
	}
	fprintf(stderr, _T("getSoundInterfaceManager...OK\n"));

	// サウンドインターフェースマネージャーインスタンス初期化
	pManager->initializeInstance();

	// リセットを行う
	pManager->reset();
	pManager->init();
	pManager->setLevelDisp(TRUE);
	//pManager->setDelay(0);

	sis.clear();
	int ifcount = pManager->getInterfaceCount();
	cout << "Found " << ifcount << " interface(s) ready." << endl << endl;
	for (int i = 0; i<ifcount; i++) {
		scciInterface* psi = new scciInterface();
		psi->psi = pManager->getInterface(i);
		psi->scs.clear();
		psi->sii = pManager->getInterfaceInfo(i);
		int count = psi->psi->getSoundChipCount();
		for (int j = 0; j<count; j++) {
			SoundChip* pchip = psi->psi->getSoundChip(j);
			psi->scs.push_back(pchip);
		}
		sis.push_back(psi);
	}
	return 0;
}

int CSCCIWrapper::getInterfaceCount()
{
	return sis.size();
}

int CSCCIWrapper::getSoundChipCount()
{
	int res = 0;
	for each (scciInterface* psi in sis)
	{
		res += psi->scs.size();
	}
	return res;
}

int CSCCIWrapper::getSoundChipCount(int ifid)
{
	if (0 <= ifid && ifid < sis.size())
	{
		return sis[ifid]->scs.size();
	}
	return 0;
}

CSCCIWrapper::~CSCCIWrapper()
{
	if (pManager) {
		// 一括開放する場合（チップ一括開放の場合）
		pManager->releaseAllSoundChip();
		pManager->releaseAllInterface();

		// サウンドインターフェースマネージャーインスタンス開放
		// FreeLibraryを行う前に必ず呼び出ししてください
		pManager->releaseInstance();
	}
	if (hScci) {
		FreeLibrary(hScci);
	}
}

SoundChip* CSCCIWrapper::getSoundChipFromString(TCHAR* name)
{
	if (pManager) {
		if (StrChr(name, _T(':')) != 0) {
			int ifid = -1;
			int devid = -1;
			sscanf_s(name, _T("%i:%i"), &ifid, &devid);
			//SoundInterface* psi = pManager->getInterface(ifid);
			return getSoundChipFromId(ifid, devid);
		}
		else {
			SC_CHIP_TYPE type = getScChipTypeFromName(name);
			for (int i = 0; i<sis.size(); i++) {
				for (int j = 0; j<sis[i]->scs.size(); j++) {
					if (sis[i]->scs[j]->getSoundChipType() == type) {
						return sis[i]->scs[j];
					}
				}
			}
		}
	}
	return 0;
}

SoundInterface* CSCCIWrapper::getSoundInterface(int ifid)
{
	if (ifid >= 0 && ifid < sis.size()) {
		return sis[ifid]->psi;
	}
	return 0;
}

scciInterface* CSCCIWrapper::getScciInterface(int ifid)
{
	if (ifid >= 0 && ifid < sis.size()) {
		return sis[ifid];
	}
	return 0;
}

int CSCCIWrapper::getInterfaceIDFromChip(SoundChip* pchip)
{
	for (int i = 0; i<sis.size(); i++) {
		for (int j = 0; j<sis[i]->scs.size(); j++) {
			if (sis[i]->scs[j] == pchip) {
				return i;
			}
		}
	}
	return -1;
}

int CSCCIWrapper::getInterfaceIDFromIF(SoundInterface* psi)
{
	for (int i = 0; i<sis.size(); i++) {
		for (int j = 0; j<sis[i]->scs.size(); j++) {
			if (sis[i]->psi == psi) {
				return i;
			}
		}
	}
	return -1;
}

int CSCCIWrapper::getInterfaceDesc(TCHAR* str, int len, int id)
{
	if (id < sis.size()) {
		sprintf_s(str, len, _T("%s"), sis[id]->sii->cInterfaceName);
	}
	return -1;
}


int CSCCIWrapper::getSlotIDFromChip(int ifid, SoundChip* pchip)
{
	if (ifid >= 0 && ifid < sis.size()) {
		for (int j = 0; j < sis[ifid]->scs.size(); j++) {
			if (sis[ifid]->scs[j] == pchip) {
				return j;
			}
		}
	}
	return -1;
}

SoundChip* CSCCIWrapper::getSoundChipFromId(int ifid, int devid)
{
	if (ifid >= 0 && devid >= 0 && ifid < sis.size() && devid < sis[ifid]->scs.size() && devid >= 0) {
		return sis[ifid]->scs[devid];
	}
	return 0;
}

SoundChip* CSCCIWrapper::getSoundChipFromType(SC_CHIP_TYPE type)
{
	return pManager->getSoundChip(type, 0);
}

const SC_CHIP_TYPE CSCCIWrapper::getScChipTypeFromName(TCHAR* name)
{
	for (int i = 0; ROM::devmap[i].devid != DEVICE_NONE; i++) {
		if (lstrcmp(ROM::devmap[i].chipname, name) == 0 || lstrcmp(ROM::devmap[i].chipcode, name) == 0) {
			return ROM::devmap[i].sctype;
		}
	}
	return SC_CHIP_TYPE::SC_TYPE_NONE;
}

const SC_CHIP_TYPE CSCCIWrapper::getScChipType(UINT8 devid)
{
	for (int i = 0; ROM::devmap[i].devid != DEVICE_NONE; i++) {
		if (ROM::devmap[i].devid == devid) {
			return ROM::devmap[i].sctype;
		}
	}
	return SC_CHIP_TYPE::SC_TYPE_NONE;
}

void CSCCIWrapper::InitialClear()
{
	if (pManager) {
		pManager->reset();
	}
}