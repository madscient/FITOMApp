#pragma once
#include <vector>
using namespace std;

struct scciInterface {
	SoundInterface* psi;
	SCCI_INTERFACE_INFO* sii;
	vector<SoundChip*> scs;
};

class CSCCIWrapper
{
protected:
	HMODULE	hScci;
	SoundInterfaceManager* pManager;
	vector<scciInterface*> sis;
public:
	SoundInterfaceManager* getInterfaceManager() const { return pManager; };
	int Init();
	CSCCIWrapper();
	~CSCCIWrapper();
	SoundChip* getSoundChipFromString(TCHAR* name);
	SoundChip* getSoundChipFromId(int ifid, int devid);
	SoundChip* getSoundChipFromType(SC_CHIP_TYPE type);
	SoundInterface* getSoundInterface(int ifid);
	scciInterface* getScciInterface(int ifid);
	void InitialClear();
	int getInterfaceCount();
	int getSoundChipCount();
	int getSoundChipCount(int ifid);
	int getInterfaceIDFromChip(SoundChip* pchip);
	int getInterfaceIDFromIF(SoundInterface* psi);
	int getSlotIDFromChip(int ifid, SoundChip* pchip);
	int getInterfaceDesc(TCHAR* str, int len, int id);
	static const SC_CHIP_TYPE getScChipType(UINT8 devid);
	static const SC_CHIP_TYPE getScChipTypeFromName(TCHAR* name);
};

