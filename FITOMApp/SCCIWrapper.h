#pragma once
#include <vector>
using namespace std;

class CSCCIWrapper
{
protected:
	HMODULE	hScci;
	SoundInterfaceManager* pManager;
	struct scciInterface {
		SoundInterface* psi;
		vector<SoundChip*> scs;
	};
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
	void InitialClear();
	int getInterfaceIDFromChip(SoundChip* pchip);
	int getSlotIDFromChip(int ifid, SoundChip* pchip);
	static const SC_CHIP_TYPE getScChipType(UINT8 devid);
	static const SC_CHIP_TYPE getScChipTypeFromName(TCHAR* name);
};

