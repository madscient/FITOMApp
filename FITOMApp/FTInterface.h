#pragma once
#include <vector>
#include "Port.h"
#include "../FTDI/ftd2xx.h"
#include <string>

#define BUFSIZE 65536

int InitFTInterface();

struct FTHINFO {
public:
	FTHINFO() : index(0), ftHandle(0), wptr(0), rptr(0) {};
	FTHINFO& operator=(FTHINFO& fth);
	uint32_t index;
	FT_HANDLE ftHandle;
	std::string description;
	std::string serialnumber;
	uint8_t cmdbuf[BUFSIZE];
	int wptr;
	int rptr;
};

class CFTInterface {
protected:
	BOOL bValid;
	FTHINFO FTChannel;
	virtual void BufferPush(BYTE data);
	virtual void BufferPush(BYTE* buf, uint32_t length);
public:
	CFTInterface();
	~CFTInterface();
	virtual BOOL IsValid() { return bValid; };
	virtual std::string GetInterfaceDesc();
	virtual FT_HANDLE GetHandle() { return FTChannel.ftHandle; };
	virtual FT_STATUS BufferFlush();
	//to be implemented on derived class
	virtual FT_STATUS Init() { return FT_NOT_SUPPORTED; };
	virtual void InitialClear() {};
	virtual FT_STATUS BufferedRead(uint8_t* buffer, uint32_t sizeToTransfer, uint32_t cs) { return FT_NOT_SUPPORTED; };
	virtual FT_STATUS BufferedWrite(uint8_t* buffer, uint32_t sizeToTransfer, uint32_t cs) { return FT_NOT_SUPPORTED; };
	virtual FT_STATUS FT_WriteGPIO(uint8_t dir, uint8_t value) { return FT_NOT_SUPPORTED; };
	virtual FT_STATUS Write(uint16_t addr, uint8_t data, uint32_t wait) { return FT_NOT_SUPPORTED; };
	virtual CPort* CreatePort(uint32_t slot, uint32_t addr1, uint32_t addr2, size_t maxreg) = 0;
};

class CFT232HSPI : public CFTInterface {
public:
	CFT232HSPI(FTHINFO& fth);
	virtual FT_STATUS Init();
	virtual void InitialClear();
	virtual FT_STATUS BufferedWrite(uint8_t* buffer, uint32_t sizeToTransfer, uint32_t cs);
	virtual FT_STATUS FT_WriteGPIO(uint8_t dir, uint8_t value);
	virtual CPort* CreatePort(uint32_t slot, uint32_t addr1, uint32_t addr2, size_t maxreg);
};

class CFT2232HBE : public CFTInterface {
public:
	CFT2232HBE(FTHINFO& fth);
	virtual FT_STATUS Init();
	virtual void InitialClear();
	virtual FT_STATUS BufferedWrite(uint8_t* buffer, uint32_t sizeToTransfer, uint32_t cs);
	virtual FT_STATUS FT_WriteGPIO(uint8_t dir, uint8_t value);
	virtual CPort* CreatePort(uint32_t slot, uint32_t addr1, uint32_t addr2, size_t maxreg);
};

class CFT245Rebirth : public CFTInterface {
public:
	CFT245Rebirth(FTHINFO& fth);
	virtual FT_STATUS Init();
	virtual void InitialClear();
	virtual FT_STATUS Write(uint16_t addr, uint8_t data, uint32_t wait);
	virtual CPort* CreatePort(uint32_t slot, uint32_t addr1, uint32_t addr2, size_t maxreg);
};

class CFT825Port : public CPort
{
protected:
	CFT232HSPI* pInterface;
	size_t regsize;
	uint32_t chidx;
	uint32_t csidx;
	FT_HANDLE ftHandle;
public:
	CFT825Port();
	CFT825Port(CFT232HSPI* pif, uint32_t index, uint32_t cs, size_t maxreg);
	~CFT825Port(void);
	virtual void writeBurst(uint16_t addr, BYTE* buf, size_t length);
	virtual void writeBurst(BYTE* buf, size_t length);
	virtual void write(uint16_t addr, uint16_t data);
	virtual void writeRaw(uint16_t addr, uint16_t data) { write(addr, data); };
	virtual uint8_t read(uint16_t addr);
	virtual uint8_t status();
	virtual void reset();
	virtual void flush();
	virtual int GetClock();
	virtual int GetPanpot();
	virtual std::string GetInterfaceDesc();
	virtual std::string GetDesc();
};

class CRebirthPort : public CPort
{
protected:
	CFT245Rebirth* pInterface;
	size_t regsize;
	uint32_t ctladdr;
	uint32_t dataddr;
	uint32_t slot_no;
	uint32_t regwait;
	uint32_t datwait;
	FT_HANDLE ftHandle;
public:
	CRebirthPort();
	CRebirthPort(CFT245Rebirth* pif, uint32_t slot, uint32_t addr1, uint32_t addr2, size_t maxreg);
	~CRebirthPort(void);
	virtual void write(uint16_t addr, uint16_t data);
	virtual void writeRaw(uint16_t addr, uint16_t data) { write(addr, data); };
	virtual uint8_t status();
	virtual void reset();
	virtual int GetClock();
	virtual int GetPanpot();
	virtual std::string GetInterfaceDesc();
	virtual std::string GetDesc();
	virtual uint8_t read(uint16_t addr) { return 0xff; };
};

class CHBEPort : public CPort
{
protected:
	CFT2232HBE* pInterface;
	size_t regsize;
	uint32_t addr;
	uint32_t slot;
	FT_HANDLE ftHandle;
public:
	CHBEPort();
	CHBEPort(CFT2232HBE* pif, uint32_t slot, uint32_t addr, size_t maxreg);
	~CHBEPort(void);
	virtual void write(uint16_t addr, uint16_t data);
	virtual void writeRaw(uint16_t addr, uint16_t data) { write(addr, data); };
	virtual uint8_t status();
	virtual void reset();
	virtual int GetClock();
	virtual int GetPanpot();
	virtual void GetInterfaceDesc(TCHAR* str, int len);
	virtual int GetDesc(TCHAR* str, int len);
};
