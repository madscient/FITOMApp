#pragma once
#include <vector>
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
};

class CFT232HSPI : public CFTInterface {
public:
	CFT232HSPI(FTHINFO& fth);
	virtual FT_STATUS Init();
	virtual void InitialClear();
	virtual FT_STATUS BufferedWrite(uint8_t* buffer, uint32_t sizeToTransfer, uint32_t cs);
	virtual FT_STATUS FT_WriteGPIO(uint8_t dir, uint8_t value);
};

class CFT2232HBE : public CFTInterface {
public:
	CFT2232HBE(FTHINFO& fth);
	virtual FT_STATUS Init();
	virtual void InitialClear();
	virtual FT_STATUS BufferedWrite(uint8_t* buffer, uint32_t sizeToTransfer, uint32_t cs);
	virtual FT_STATUS FT_WriteGPIO(uint8_t dir, uint8_t value);
};

class CFT245Rebirth : public CFTInterface {
public:
	CFT245Rebirth(FTHINFO& fth);
	virtual FT_STATUS Init();
	virtual void InitialClear();
	virtual FT_STATUS Write(uint16_t addr, uint8_t data, uint32_t wait);
};
