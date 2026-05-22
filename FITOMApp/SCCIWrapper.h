#pragma once
#include <vector>
#include "Port.h"
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
	static const SC_CHIP_TYPE getScChipType(uint8_t devid);
	static const SC_CHIP_TYPE getScChipTypeFromName(TCHAR* name);
};

class CSCCIPort : public CPort
{
protected:
	SoundChip* pChip;
	scciInterface* pInterface;
	size_t regsize;
public:
	CSCCIPort();
	CSCCIPort(scciInterface* pif, SoundChip* pchip, size_t maxreg);
	~CSCCIPort(void) {};
	virtual void write(uint16_t addr, uint16_t data);
	virtual void writeRaw(uint16_t addr, uint16_t data) { write(addr, data); };
	virtual uint8_t read(uint16_t addr);
	virtual uint8_t status();
	virtual void reset();
	virtual int GetClock();
	virtual int GetPanpot();
	virtual std::string GetInterfaceDesc();
	virtual std::string GetDesc();
};

